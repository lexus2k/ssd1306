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
 * @file ssd1306_i2c_wire.h SSD1306 i2c communication functions for standard Wire library
 */

#ifndef _SSD1306_I2C_WIRE_H_
#define _SSD1306_I2C_WIRE_H_

#include "hal/io.h"
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
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void ssd1306_i2cInit_Wire(uint8_t sa);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_I2C_WIRE_H_ */


