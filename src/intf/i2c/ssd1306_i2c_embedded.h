/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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

/**
 * @file ssd1306_i2c_embedded.h embedded SSD1306 i2c communication functions
 */

#ifndef _SSD1306_I2C_EMBEDDED_H_
#define _SSD1306_I2C_EMBEDDED_H_

#include "ssd1306_hal/io.h"
#include "ssd1306_i2c_conf.h"

#if defined(CONFIG_SOFTWARE_I2C_AVAILABLE) && defined(CONFIG_SOFTWARE_I2C_ENABLE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Initializes software implementation of i2c.
 * If you do not know i2c parameters, try ssd1306_i2cInit_Embedded(0,0,0).
 * @warning the function disables interrupts.
 * @param scl - i2c clock pin. Use -1 if you don't need to change default pin number
 * @param sda - i2c data pin. Use -1 if you don't need to change default pin number
 * @param sa  - i2c address of lcd display. Use 0 to leave default
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void ssd1306_i2cInit_Embedded(int8_t scl, int8_t sda, uint8_t sa);

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_I2C_EMBEDDED_H_ */


