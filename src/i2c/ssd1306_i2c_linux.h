/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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
 * @file ssd1306_i2c_linux.h SSD1306 i2c communication functions for standard Linux i2c interface
 */

#ifndef _SSD1306_I2C_LINUX_H_
#define _SSD1306_I2C_LINUX_H_

#include "hal/io.h"
#include "ssd1306_i2c_conf.h"

#ifdef SSD1306_LINUX_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

/**
 * Initializes ssd1306 library to use Linux i2c-dev interface.
 * If you do not know i2c parameters, try ssd1306_i2cInit_Linux(-1,0).
 *
 * @param busId - i2c bus id, can be -1 for default value (i2c-1 is default)
 * @param sa  - i2c address of lcd display. Use 0 to leave default
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void ssd1306_i2cInit_Linux(int8_t busId, uint8_t sa);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_I2C_TWI_H_ */


