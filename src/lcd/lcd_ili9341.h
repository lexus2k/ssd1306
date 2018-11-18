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
 * @file lcd_ili9341.h support for RGB TFT 320x240 display
 */


#ifndef _TFT_ILI9341_H_
#define _TFT_ILI9341_H_

#include "ssd1306_hal/io.h"
#include "lcd/lcd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * @brief Sets GDRAM autoincrement mode
 *
 * Sets GDRAM autoincrement mode. By default, to make
 * ssd1306_xxx functions compatible with RGB oled display,
 * RGB oled is initialized in vertical auto-increment mode.
 * But for pure rbg oled operations horizontal auto-increment mode is more suitable.
 * So, if you're going to use NanoCanvas8 functions, please call
 * ili9341_setMode(0) prior to using pure RGB methods.
 *
 * @param mode 0 or 1
 * @deprecated Use ssd1306_setMode() instead.
 */
void        ili9341_setMode(lcd_mode_t mode);

/**
 * @brief Inits 240x320 RGB OLED display (based on ili9341 controller).
 *
 * Inits 240x320 RGB OLED display (based on ili9341 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ili9341_240x320_init(void);

/**
 * @brief Inits 240x320 RGB TFT display over spi (based on ili9341 controller).
 *
 * Inits 240x320 RGB TFT display over spi (based on ili9341 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ili9341_240x320_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @}
 */

/**
 * @defgroup ili9341_API ili9341: ili9341 control functions
 * @{
 */

/**
 * @brief Sets screen orientation (rotation)
 *
 * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @note works only with ili9341 display
 */
void ili9341_setRotation(uint8_t rotation);

/**
 * @brief Sets rotation of all output functions
 *
 * Sets rotation of all output functions. 0 - no rotation, 1 - change by 90 degrees
 * Actually doesn't change screen orientation, only rotates primitives being output
 *
 * @param on enable (1) of disable (0)
 * @warning experimental feature, may work incorrectly
 */
void ili9341_rotateOutput(uint8_t on);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _TFT_ILI9141_H_
