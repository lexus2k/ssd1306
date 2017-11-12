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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inits display interface to use i2c bus.
 * The function automatically selects available type of i2c implementation
 * 1. Wire library
 * 2. sw i2c implementation
 * In case of using Wire library this function calls Wire.begin() and
 * sets speed to fast i2c (400kHz). If you prefer to use your own Wire settings
 * or avoid reinitializing of Wire library, please use ssd1306_i2cInit_Wire().
 */
void         ssd1306_i2cInit();

/**
 * Switches i2c chip to command transmission mode.
 */
void         ssd1306_i2cCommandStart();

/**
 * Switches i2c chip to data transmission mode.
 */
void         ssd1306_i2cDataStart();

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_I2C_H_
