/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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

#include "ssd1306_i2c_wire.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_i2c_conf.h"
#include "ssd1306_i2c.h"

#ifdef SSD1306_WIRE_SUPPORTED

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

void ssd1306_i2cConfigure_Wire(int8_t scl, int8_t sda)
{
#if defined(ESP8266) || defined(ESP32) || defined(ESP31B)
    if ((scl>=0) && (sda >=0))
    {
        Wire.begin(sda, scl);
    }
    else
#endif
    {
        Wire.begin();
    }
    #ifdef SSD1306_WIRE_CLOCK_CONFIGURABLE
        Wire.setClock(400000);
    #endif
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
    if (s_bytesWritten >= (BUFFER_LENGTH >> 1))
#else
    if (s_bytesWritten >= (USI_BUF_SIZE -2))
#endif
    {
        ssd1306_i2cStop_Wire();
        ssd1306_i2cDataStart();
        /* Commands never require many bytes. Thus assume that user tries to send data */
    }
    Wire.write(data);
    s_bytesWritten++;
}

static void ssd1306_i2cClose_Wire()
{
}

void ssd1306_i2cInit_Wire(uint8_t sa)
{
    if (sa) s_sa = sa;
    ssd1306_startTransmission = ssd1306_i2cStart_Wire;
    ssd1306_endTransmission = ssd1306_i2cStop_Wire;
    ssd1306_sendByte = ssd1306_i2cSendByte_Wire;
    ssd1306_closeInterface = ssd1306_i2cClose_Wire;
    ssd1306_commandStart = ssd1306_i2cCommandStart;
    ssd1306_dataStart = ssd1306_i2cDataStart;
}

#endif


