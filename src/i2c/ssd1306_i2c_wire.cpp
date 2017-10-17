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


#include "ssd1306_i2c.h"

#include "intf/ssd1306_interface.h"
#include "ssd1306_i2c_conf.h"

#if !defined(SSD1306_EMBEDDED_I2C)
/* STANDARD branch */
    #include <Wire.h>

uint8_t s_bytesWritten = 0;

void ssd1306_i2cStart(void)
{
    Wire.beginTransmission(SSD1306_SA);
    s_bytesWritten = 0;
}

void ssd1306_i2cStop(void)
{
    Wire.endTransmission();
}

/**
 * Inputs: SCL is LOW, SDA is has no meaning
 * Outputs: SCL is LOW
 */
void ssd1306_i2cSendByte(uint8_t data)
{
    // Do not write too many bytes for standard Wire.h. It may become broken
    if (s_bytesWritten >= (BUFFER_LENGTH >> 1))
    {
        ssd1306_i2cStop();
        ssd1306_i2cStart();
        /* Commands never require many bytes. Thus assume that user tries to send data */
        Wire.write(0x40);
        s_bytesWritten++;
    }        
    Wire.write(data);
    s_bytesWritten++;
}


#endif


