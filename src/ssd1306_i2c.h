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

#ifndef _SSD1306_I2C_H_
#define _SSD1306_I2C_H_

#include <Arduino.h>

#ifndef SSD1306_SA
    /* Write command will be SSD1306_SA<<1 and read will be SSD1306_SA<<1 | 1 */
    #define SSD1306_SA    0x3C  // Slave address
#endif

void         ssd1306_i2cStart();
void         ssd1306_i2cStop();
void         ssd1306_i2cSendByte(uint8_t data);
void         ssd1306_i2cSendCommand(uint8_t data);
void         ssd1306_i2cDataStart();

// ----------------------------------------------------------------------------
#endif // _SSD1306_I2C_H_
