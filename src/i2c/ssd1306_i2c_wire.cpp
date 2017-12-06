/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ssd1306_i2c_wire.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_i2c_conf.h"
#include "ssd1306_i2c.h"

#ifdef SSD1306_WIRE_SUPPORTED

#include <Wire.h>

static uint8_t s_bytesWritten = 0;
static uint8_t s_sa = SSD1306_SA;

void ssd1306_i2cStart_Wire(void)
{
    Wire.beginTransmission(s_sa);
    s_bytesWritten = 0;
}

void ssd1306_i2cStop_Wire(void)
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
void ssd1306_i2cSendByte_Wire(uint8_t data)
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
        ssd1306_i2cStart_Wire();
        /* Commands never require many bytes. Thus assume that user tries to send data */
        Wire.write(0x40);
        s_bytesWritten++;
    }        
    Wire.write(data);
    s_bytesWritten++;
}

void ssd1306_i2cInit_Wire(uint8_t sa)
{
    if (sa) s_sa = sa;
    ssd1306_startTransmission = ssd1306_i2cStart_Wire;
    ssd1306_endTransmission = ssd1306_i2cStop_Wire;
    ssd1306_sendByte = ssd1306_i2cSendByte_Wire;
    ssd1306_commandStart = ssd1306_i2cCommandStart;
    ssd1306_dataStart = ssd1306_i2cDataStart;
}

#endif


