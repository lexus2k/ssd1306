/*
    Copyright (C) 2017 Alexey Dynda

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
 * @file ssd1306_i2c_wire.h SSD1306 i2c communication functions for standard Wire library
 */

#ifndef _SSD1306_I2C_WIRE_H_
#define _SSD1306_I2C_WIRE_H_

#include <stdint.h>
#include "ssd1306_i2c_conf.h"

#ifdef SSD1306_WIRE_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

/**
 * Configures speed for standard Wire library (via Wire.begin()).
 * This function is called by ssd1306_i2cInit().
 * @param scl - i2c clock pin. Use -1 if you don't need to change default pin number
 * @param sda - i2c data pin. Use -1 if you don't need to change default pin number
 *
 * @note scl and sda parameters depend on used hardware. For many hardware boards these
 * parameters do not have any effect. ESP8266 allows to specify these parameters
 */
void ssd1306_i2cConfigure_Wire(int8_t scl, int8_t sda);

/**
 * Initializes ssd1306 library to use Wire lib for i2c.
 * If you do not know i2c parameters, try ssd1306_i2cInit_Wire(0).
 * SCL and SDA pins depend on platform and Wire library implementation.
 * @param sa  - i2c address of lcd display. Use 0 to leave default
 */
void ssd1306_i2cInit_Wire(uint8_t sa);

/**
 * Starts i2c communication with lcd display via Wire library.
 */
void ssd1306_i2cStart_Wire();

/**
 * Stops i2c communication.
 */
void ssd1306_i2cStop_Wire();

/**
 * Sends single byte to i2c device using Wire library.
 * @param data - byte to send
 */
void ssd1306_i2cSendByte_Wire(uint8_t data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_I2C_WIRE_H_ */


