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
 * @file ssd1306_128x32.h support for OLED 128x32 display
 */


#ifndef _SSD1306_128X32_H_
#define _SSD1306_128X32_H_

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
