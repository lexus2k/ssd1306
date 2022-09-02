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

#if defined(SSD1306_ENERGIA_PLATFORM)

#include "intf/ssd1306_interface.h"
#include <Wire.h> // Energia I2C implementation (Arduino-like)

////////////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)
static uint8_t s_bytesWritten = 0;
static uint8_t s_i2c_addr = 0x3C;
static TwoWire *s_i2c = nullptr;

static void platform_i2c_start(void)
{
    s_i2c->beginTransmission(s_i2c_addr);
    s_bytesWritten = 0;
}

static void platform_i2c_stop(void)
{
    s_i2c->endTransmission();
}

static void platform_i2c_send(uint8_t data)
{
    // Do not write too many bytes for standard Wire.h. It may become broken (Arduino)
    if (s_bytesWritten >= (BUFFER_LENGTH - 2))
    {
        platform_i2c_stop();
        platform_i2c_start();
        platform_i2c_send(0x40);
        /* Commands never require many bytes. Thus assume that user tries to send data (Arduino) */
    }
    s_i2c->write(data);
    s_bytesWritten++;
}

static void platform_i2c_close(void)
{
    // ... free all i2c resources here
}

static void platform_i2c_send_buffer(const uint8_t *data, uint16_t len)
{
    while (len--)
    {
        platform_i2c_send(*data);
        data++;
    }
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, ssd1306_platform_i2cConfig_t * cfg)
{
    s_i2c = &Wire;
    s_i2c->begin();

    if (addr) s_i2c_addr = addr;
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = &platform_i2c_start;
    ssd1306_intf.stop  = &platform_i2c_stop;
    ssd1306_intf.send  = &platform_i2c_send;
    ssd1306_intf.close = &platform_i2c_close;
    ssd1306_intf.send_buffer = &platform_i2c_send_buffer;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// !!! PLATFORM SPI IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#include "intf/spi/ssd1306_spi.h"
#include "lcd/lcd_common.h"
#include <SPI.h>

static void platform_spi_start(void)
{
    digitalWrite(s_ssd1306_cs, LOW);
}

static void platform_spi_stop(void)
{
    if (ssd1306_lcd.type == LCD_TYPE_PCD8544)
    {
        digitalWrite(s_ssd1306_dc, LOW);
        SPI.transfer(0x00); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                            // ssd1306 E3h is NOP command
    }
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs, HIGH);
    }
}

static void platform_spi_send(uint8_t data)
{
    SPI.transfer(data);
}

static void platform_spi_close(void)
{
    // ... free all spi resources here
}

static void platform_spi_send_buffer(const uint8_t *data, uint16_t len)
{
    while (len--)
    {
        SPI.transfer(*data);
        data++;
    };
}

void ssd1306_platform_spiInit(int8_t busId,
                              int8_t cesPin,
                              int8_t dcPin)
{
    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if (cesPin>=0) s_ssd1306_cs = cesPin;
    if (dcPin>=0) s_ssd1306_dc = dcPin;
    SPI.begin();
    ssd1306_intf.spi = 1;
    ssd1306_intf.start = &platform_spi_start;
    ssd1306_intf.stop  = &platform_spi_stop;
    ssd1306_intf.send  = &platform_spi_send;
    ssd1306_intf.close = &platform_spi_close;
    ssd1306_intf.send_buffer = &platform_spi_send_buffer;
    // init your interface here
    //...
}
#endif

#endif // SSD1306_ENERGIA_PLATFORM
