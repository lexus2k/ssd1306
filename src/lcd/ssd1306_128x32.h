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
 * @file ssd1306_128x32.h support for OLED 128x32 display
 */


#ifndef _SSD1306_128X32_H_
#define _SSD1306_128X32_H_

#include <Arduino.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_DISPLAY_API LCD Display control functions
 * @{
 */

/**
 * @brief Inits 128x32 OLED display over i2c (based on SSD1306 controller).
 *
 * Inits 128x32 OLED display over i2c (based on SSD1306 controller)
 * This function uses hardcoded pins for i2c communication, depending on your hardware.
 * If you use non-standard pins in your project, please perform call ssd1306_i2cInitEx() and
 * ssd1306_128x32_init().
 */
void         ssd1306_128x32_i2c_init();

/**
 * @brief Inits 128x32 OLED display (based on ssd1306 controller).
 *
 * Inits 128x32 OLED display (based on ssd1306 controller)
 * spi or i2c bus must be initialized prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ssd1306_128x32_init();

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_128X32_H_
