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
 * @file ssd1306_128x64.h support for OLED 128x64 display
 */


#ifndef _SSD1306_128X64_H_
#define _SSD1306_128X64_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_DISPLAY_API LCD Display control functions
 * @{
 */

/**
 * @brief Inits 128x64 OLED display (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display (based on SSD1306 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ssd1306_128x64_init();

/**
 * @brief Inits 128x64 OLED display over i2c (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display over i2c (based on SSD1306 controller)
 * This function uses hardcoded pins for i2c communication, depending on your hardware.
 * If you use non-standard pins in your project, please perform call ssd1306_i2cInitEx() and
 * ssd1306_128x64_init().
 */
void         ssd1306_128x64_i2c_init();

/**
 * @brief Inits 128x64 OLED display over spi (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display over spi (based on SSD1306 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1306_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @copydoc ssd1306_128x64_i2c_init
 */
void         ssd1306_init() __attribute__((deprecated));

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_128X64_H_
