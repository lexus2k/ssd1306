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
 * @file oled_template.h support for TEMPLATE OLED WxH display
 */


#ifndef _OLED_TEMPLATE_H_
#define _OLED_TEMPLATE_H_

#include "ssd1306_hal/io.h"
#include "lcd/lcd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup TEMPLATE_OLED_API TEMPLATE: template control functions
 * @{
 */

// PLACE ANY OLED SPECIFIC FUNCTIONS HERE

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
 * ssd1306_xxx functions compatible with TEMPLATE oled display,
 * TEMPLATE oled is initialized in vertical auto-increment mode.
 * But for pure TEMPLATE oled operations horizontal auto-increment mode is more suitable.
 * So, if you're going to use NanoCanvas8 functions, please call
 * template_setMode(0) prior to using pure TEMPLATE oled methods.
 *
 * @param mode 0 or 1
 * @deprecated Use ssd1306_setMode() instead.
 */
void        template_setMode(lcd_mode_t mode);

/**
 * @brief Inits WxH TEMPLATE OLED display (based on TEMPLATE controller).
 *
 * Inits WxH TEMPLATE OLED display (based on TEMPLATE controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         template_WxH_init(void);

/**
 * @brief Inits WxH TEMPLATE OLED display over spi (based on TEMPLATE controller).
 *
 * Inits WxH TEMPLATE OLED display over spi (based on TEMPLATE controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         template_WxH_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _OLED_TEMPLATE_H_
