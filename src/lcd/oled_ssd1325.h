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
 * @file oled_ssd1325.h support for SSD1325 OLED 128x64 display
 */


#ifndef _OLED_SSD1325_H_
#define _OLED_SSD1325_H_

#include "ssd1306_hal/io.h"
#include "lcd/lcd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SSD1325_OLED_API SSD1325: ssd1325 control functions
 * @{
 */


/**
 * @}
 */

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * @brief Sets GDRAM autoincrement mode
 *
 * Sets GDRAM autoincrement mode. By default, to make
 * ssd1306_xxx functions compatible with SSD1325 oled display,
 * SSD1325 oled is initialized in vertical auto-increment mode.
 * But for pure SSD1325 oled operations horizontal auto-increment mode is more suitable.
 * So, if you're going to use NanoCanvas8 functions, please call
 * ssd1325_setMode(0) prior to using pure SSD1325 oled methods.
 *
 * @param mode 0 or 1
 * @deprecated Use ssd1306_setMode() instead.
 */
void ssd1325_setMode(lcd_mode_t mode);

/**
 * @brief Inits 128x64 SSD1325 OLED display (based on SSD1325 controller).
 *
 * Inits 128x64 SSD1325 OLED display (based on SSD1325 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void ssd1325_128x64_init(void);

/**
 * @brief Inits 128x64 SSD1325 OLED display over spi (based on SSD1325 controller).
 *
 * Inits 128x64 SSD1325 OLED display over spi (based on SSD1325 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void ssd1325_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1325_H_
