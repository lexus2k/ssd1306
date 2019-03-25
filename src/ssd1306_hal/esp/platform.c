/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "ssd1306_hal/io.h"

#if defined(SSD1306_ESP_PLATFORM)

#include "intf/ssd1306_interface.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#if 1
// TODO: To complete support. Any help is welcome
int  digitalRead(int pin)   // digitalRead()
{
    return gpio_get_level(pin);
}

void digitalWrite(int pin, int level)  // digitalWrite()
{
    gpio_set_level(pin, level);
}

void pinMode(int pin, int mode)
{
    if (mode == INPUT)
        gpio_set_direction(pin, GPIO_MODE_INPUT);
    else if (mode == OUTPUT)
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

uint32_t millis(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void delay(uint32_t ms)     // delay()
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////
// !!! PLATFORM I2C IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)

#include <stdio.h>
#include "driver/i2c.h"

static uint8_t s_i2c_addr = 0x3C;
static int8_t s_bus_id;

static i2c_cmd_handle_t s_cmd_handle;

static void platform_i2c_start(void)
{
    // ... Open i2c channel for your device with specific s_i2c_addr
    s_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(s_cmd_handle);
    i2c_master_write_byte(s_cmd_handle, ( s_i2c_addr << 1 ) | I2C_MASTER_WRITE, 0x1);
}

static void platform_i2c_stop(void)
{
    // ... Complete i2c communication
    i2c_master_stop(s_cmd_handle);
    /*esp_err_t ret =*/ i2c_master_cmd_begin(s_bus_id, s_cmd_handle, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(s_cmd_handle);
}

static void platform_i2c_send(uint8_t data)
{
    // ... Send byte to i2c communication channel
    i2c_master_write_byte(s_cmd_handle, data, 0x1);
}

static void platform_i2c_close(void)
{
    // ... free all i2c resources here
    i2c_driver_delete(s_bus_id);
}

static void platform_i2c_send_buffer(const uint8_t *data, uint16_t len)
{
    // ... Send len bytes to i2c communication channel here
    while (len--)
    {
        platform_i2c_send(*data);
        data++;
    }
//    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, ssd1306_platform_i2cConfig_t * cfg)
{
    if (addr) s_i2c_addr = addr;
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = &platform_i2c_start;
    ssd1306_intf.stop  = &platform_i2c_stop;
    ssd1306_intf.send  = &platform_i2c_send;
    ssd1306_intf.close = &platform_i2c_close;
    ssd1306_intf.send_buffer = &platform_i2c_send_buffer;
    // init your interface here
    if ( busId < 0) busId = I2C_NUM_1;
    s_bus_id = busId;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = cfg->sda >= 0 ? cfg->sda : 21;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = cfg->scl >= 0 ? cfg->scl : 22;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 400000; //I2C_EXAMPLE_MASTER_FREQ_HZ;
    i2c_param_config(s_bus_id, &conf);
    i2c_driver_install(s_bus_id, conf.mode, 0, 0, 0);
//                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
//                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// !!! PLATFORM SPI IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#include "intf/spi/ssd1306_spi.h"
#include "driver/spi_master.h"

// Store spi handle globally for all spi callbacks
static spi_device_handle_t s_spi;
static int8_t s_spi_bus_id;
// s_first_spi_session is used for delayed spi initialization.
// Some oled displays have slow max SPI speed, so display init function can change
// spi frequency s_ssd1306_spi_clock. Register device, only when frequency is known.
static uint8_t s_first_spi_session = 0;

static void platform_spi_start(void)
{
    // ... Open spi channel for your device with specific s_ssd1306_cs, s_ssd1306_dc
    if (s_first_spi_session)
    {
        spi_device_interface_config_t devcfg=
        {
            .clock_speed_hz = s_ssd1306_spi_clock,
            .mode=0,
            .spics_io_num=s_ssd1306_cs,
            .queue_size=7,       // max 7 transactions at a time
        };
        spi_bus_add_device(s_spi_bus_id ? VSPI_HOST : HSPI_HOST, &devcfg, &s_spi);
        s_first_spi_session = 0;
    }
}

static void platform_spi_stop(void)
{
    // ... Complete spi communication
    // no complete actions required for this implementation
}

static void platform_spi_send(uint8_t data)
{
    // ... Send byte to spi communication channel
    // We do not care here about DC line state, because
    // ssd1306 library already set DC pin via ssd1306_spiDataMode() before call to send().
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;          // 8 bits
    t.tx_buffer=&data;
    spi_device_transmit(s_spi, &t);  // Transmit!
}

static void platform_spi_close(void)
{
    // ... free all spi resources here
    if (!s_first_spi_session)
    {
        spi_bus_remove_device( s_spi );
    }
    spi_bus_free( s_spi_bus_id ? VSPI_HOST : HSPI_HOST );
}

static void platform_spi_send_buffer(const uint8_t *data, uint16_t len)
{
    // ... Send len bytes to spi communication channel here
    while (len)
    {
        size_t sz = len > 32 ? 32: len;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length=8*sz;          // 8 bits
        t.tx_buffer=data;
        spi_device_transmit(s_spi, &t);
        data+=sz;
        len-=sz;
    }
}

void ssd1306_platform_spiInit(int8_t busId,
                              int8_t cesPin,
                              int8_t dcPin)
{
    // Use VSPI by default
    if (busId < 0) busId = 1;
    s_spi_bus_id = busId;

    // If cesPin is not provided, select by default
    if (cesPin < 0)
    {
        cesPin = s_spi_bus_id ? 5 : 15;
    }
    s_ssd1306_cs = cesPin;
    if (dcPin>=0) s_ssd1306_dc = dcPin;

    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);

    ssd1306_intf.spi = 1;
    ssd1306_intf.start = &platform_spi_start;
    ssd1306_intf.stop  = &platform_spi_stop;
    ssd1306_intf.send  = &platform_spi_send;
    ssd1306_intf.close = &platform_spi_close;
    ssd1306_intf.send_buffer = &platform_spi_send_buffer;

    // init your interface here
    spi_bus_config_t buscfg=
    {
        .miso_io_num= s_spi_bus_id ? 19 : 12,
        .mosi_io_num= s_spi_bus_id ? 23 : 13,
        .sclk_io_num= s_spi_bus_id ? 18 : 14,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=32
    };
    spi_bus_initialize(s_spi_bus_id ? VSPI_HOST : HSPI_HOST, &buscfg, 0); // 0 -no dma
    s_first_spi_session = 1;
}
#endif

#endif // SSD1306_ESP_PLATFORM
