/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

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
 * @file lcd_il9163.h support for RGB TFT 128x128 display
 */


#ifndef _TFT_IL9163_H_
#define _TFT_IL9163_H_

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
 * il9163_setMode(0) prior to using pure RGB methods.
 *
 * @param mode 0 or 1
 * @deprecated Use ssd1306_setMode() instead.
 */
void        il9163_setMode(lcd_mode_t mode);

/**
 * @brief Inits 128x128 RGB OLED display (based on il9163 controller).
 *
 * Inits 128x128 RGB OLED display (based on il9163 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         il9163_128x128_init(void);

/**
 * @brief Sets screen offset (refer to datasheet of your display)
 *
 * Set offset for the display
 * @param x offset in pixels
 * @param y offset in pixels
 */
void         il9163_setOffset(lcdint_t x, lcdint_t y);

/**
 * @brief Inits 128x160 RGB OLED display (based on st7735 controller).
 *
 * Inits 128x160 RGB OLED display (based on st7735 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         st7735_128x160_init(void);

/**
 * @brief Inits 128x128 RGB TFT display over spi (based on il9163 controller).
 *
 * Inits 128x128 RGB TFT display over spi (based on il9163 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         il9163_128x128_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @brief Inits 128x160 RGB TFT display over spi (based on st7735 controller).
 *
 * Inits 128x160 RGB TFT display over spi (based on st7735 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         st7735_128x160_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @}
 */

/**
 * @defgroup IL9163_ST7734_API IL9163/ST7735: il9163/st7735 control functions
 * @{
 */

/**
 * @brief Sets screen orientation (rotation)
 *
 * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @note works only with IL9163 display
 */
void il9163_setRotation(uint8_t rotation);

/**
 * @brief Sets screen orientation (rotation)
 *
 * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @note works only with ST7735 display
 */
#define st7735_setRotation il9163_setRotation

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _TFT_IL9163_H_
