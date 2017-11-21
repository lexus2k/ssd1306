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
 * @file ssd1306_i2c_embedded.h embedded SSD1306 i2c communication functions
 */

#ifndef _SSD1306_I2C_EMBEDDED_H_
#define _SSD1306_I2C_EMBEDDED_H_

#include <stdint.h>
#include "ssd1306_i2c_conf.h"

#ifdef SSD1306_I2C_SW_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

/**
 * Initializes software implementation of i2c.
 * If you do not know i2c parameters, try ssd1306_i2cInit_Embedded(0,0,0).
 * @warning the function disables interrupts.
 * @param scl - i2c clock pin. Use -1 if you don't need to change default pin number
 * @param sda - i2c data pin. Use -1 if you don't need to change default pin number
 * @param sa  - i2c address of lcd display. Use 0 to leave default
 */
void ssd1306_i2cInit_Embedded(int8_t scl, int8_t sda, uint8_t sa);

/**
 * Starts i2c communication with lcd display using software implementation of i2c interface.
 */
void ssd1306_i2cStart_Embedded(void);

/**
 * Stops i2c communication
 * @note the function enables interrupts back.
 */
void ssd1306_i2cStop_Embedded(void);

/**
 * Sends single byte to i2c device using software implementation.
 * @param data - byte to send
 */
void ssd1306_i2cSendByte_Embedded(uint8_t data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_I2C_EMBEDDED_H_ */


