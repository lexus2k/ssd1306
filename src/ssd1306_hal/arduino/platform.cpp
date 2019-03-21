/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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

#include "intf/ssd1306_interface.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi.h"
#include "lcd/lcd_common.h"

#if defined(ARDUINO)

//////////////////////////////////////////////////////////////////////////////////
//                        ARDUINO I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && \
    defined(CONFIG_PLATFORM_I2C_ENABLE)

#include <Wire.h>

static uint8_t s_bytesWritten = 0;
static uint8_t s_sa = SSD1306_SA;

static void ssd1306_i2cStart_Wire(void)
{
    Wire.beginTransmission(s_sa);
    s_bytesWritten = 0;
}

static void ssd1306_i2cStop_Wire(void)
{
    Wire.endTransmission();
}

/**
 * Inputs: SCL is LOW, SDA is has no meaning
 * Outputs: SCL is LOW
 */
static void ssd1306_i2cSendByte_Wire(uint8_t data)
{
    // Do not write too many bytes for standard Wire.h. It may become broken
#if defined(ESP32) || defined(ESP31B)
    if (s_bytesWritten >= (I2C_BUFFER_LENGTH >> 4))
#elif defined(ARDUINO_ARCH_SAMD)
    if (s_bytesWritten >= 64)
#elif defined(BUFFER_LENGTH)
    if (s_bytesWritten >= (BUFFER_LENGTH - 2))
#elif defined(SERIAL_BUFFER_LENGTH)
    if (s_bytesWritten >= (SERIAL_BUFFER_LENGTH - 2))
#elif defined(USI_BUF_SIZE)
    if (s_bytesWritten >= (USI_BUF_SIZE -2))
#else
    if ( Wire.write(data) != 0 )
    {
        s_bytesWritten++;
        return;
    }
#endif
    {
        ssd1306_i2cStop_Wire();
        ssd1306_i2cStart_Wire();
        ssd1306_i2cSendByte_Wire(0x40);
        /* Commands never require many bytes. Thus assume that user tries to send data */
    }
    Wire.write(data);
    s_bytesWritten++;
}

static void ssd1306_i2cSendBytes_Wire(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        ssd1306_i2cSendByte_Wire(*buffer);
        buffer++;
    }
}

static void ssd1306_i2cClose_Wire()
{
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, ssd1306_platform_i2cConfig_t * cfg)
{
#if defined(ESP8266) || defined(ESP32) || defined(ESP31B)
    if ((cfg->scl >= 0) && (cfg->sda >=0))
    {
        Wire.begin(cfg->sda, cfg->scl);
    }
    else
#endif
    {
        Wire.begin();
    }
    #ifdef SSD1306_WIRE_CLOCK_CONFIGURABLE
        Wire.setClock(400000);
    #endif

    if (addr) s_sa = addr;
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = ssd1306_i2cStart_Wire;
    ssd1306_intf.stop = ssd1306_i2cStop_Wire;
    ssd1306_intf.send = ssd1306_i2cSendByte_Wire;
    ssd1306_intf.send_buffer = ssd1306_i2cSendBytes_Wire;
    ssd1306_intf.close = ssd1306_i2cClose_Wire;
}

#endif // CONFIG_PLATFORM_I2C_AVAILABLE

//////////////////////////////////////////////////////////////////////////////////
//                        ARDUINO SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

/* STANDARD branch */
#include <SPI.h>

static void ssd1306_spiClose_hw()
{
}

static void ssd1306_spiStart_hw()
{
    /* anyway, oled ssd1331 cannot work faster, clock cycle should be > 150ns: *
     * 1s / 150ns ~ 6.7MHz                                                     */
    SPI.beginTransaction(SPISettings(s_ssd1306_spi_clock, MSBFIRST, SPI_MODE0));
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs,LOW);
    }
}

static void ssd1306_spiStop_hw()
{
    if (ssd1306_lcd.type == LCD_TYPE_PCD8544)
    {
        digitalWrite(s_ssd1306_dc, LOW);
        SPI.transfer( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                              // ssd1306 E3h is NOP command
    }
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs, HIGH);
    }
    SPI.endTransaction();
}

static void ssd1306_spiSendByte_hw(uint8_t data)
{
    SPI.transfer(data);
}

static void ssd1306_spiSendBytes_hw(const uint8_t *buffer, uint16_t size)
{
    /* Do not use SPI.transfer(buffer, size)! this method corrupts buffer content */
    while (size--)
    {
        SPI.transfer(*buffer);
        buffer++;
    };
}

void ssd1306_platform_spiInit(int8_t busId, int8_t cesPin, int8_t dcPin)
{
    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if (cesPin) s_ssd1306_cs = cesPin;
    if (dcPin) s_ssd1306_dc = dcPin;
    SPI.begin();
    ssd1306_intf.spi = 1;
    ssd1306_intf.start = ssd1306_spiStart_hw;
    ssd1306_intf.stop = ssd1306_spiStop_hw;
    ssd1306_intf.send = ssd1306_spiSendByte_hw;
    ssd1306_intf.send_buffer = ssd1306_spiSendBytes_hw;
    ssd1306_intf.close = ssd1306_spiClose_hw;
}

#endif // CONFIG_PLATFORM_SPI_AVAILABLE

#endif // ARDUINO
