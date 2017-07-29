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
/**
 * @file ssd1306_i2c.h SSD1306 i2c communication functions
 */


#ifndef _SSD1306_I2C_H_
#define _SSD1306_I2C_H_

#include "ssd1306_i2c_conf.h"
#include <Arduino.h>

/**
 * Informs I2C device about data to be sent
 */
void         ssd1306_i2cStart();

/**
 * Completes I2C transaction
 */
void         ssd1306_i2cStop();

/**
 * Sends byte to I2C device
 * @param data - byte to send
 */
void         ssd1306_i2cSendByte(uint8_t data);

// ----------------------------------------------------------------------------
#endif // _SSD1306_I2C_H_
