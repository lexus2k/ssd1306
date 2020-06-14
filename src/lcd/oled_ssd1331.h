/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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
 * @file oled_ssd1331.h support for RGB OLED 96x64 display
 */


#ifndef _OLED_SSD1331_H_
#define _OLED_SSD1331_H_

#include "ssd1306_hal/io.h"
#include "lcd/lcd_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SSD1331_API SSD1331: ssd1331 control functions
 * @{
 */

/**
 * @brief Sets screen orientation (rotation)
 *
 * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
 * @note works only with ssd1331 display
 */
void ssd1331_setRotation(uint8_t rotation);

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
 * ssd1306_xxx functions compatible with RGB oled display,
 * RGB oled is initialized in vertical auto-increment mode.
 * But for pure rbg oled operations horizontal auto-increment mode is more suitable.
 * So, if you're going to use NanoCanvas8 functions, please call
 * ssd1331_setMode(0) prior to using pure RGB methods.
 *
 * @param mode 0 or 1
 * @deprecated Use ssd1306_setMode() instead.
 */
void        ssd1331_setMode(lcd_mode_t mode);

/**
 * @brief Inits 96x64 RGB OLED display (based on SSD1331 controller).
 *
 * Inits 96x64 RGB OLED display (based on SSD1331 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ssd1331_96x64_init(void);

/**
 * @brief Inits 96x64 RGB OLED display over spi in 8-bit mode (based on SSD1331 controller).
 *
 * Inits 96x64 RGB OLED display over spi in 8-bit mode (based on SSD1331 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1331_96x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @brief Inits 96x64 RGB OLED display over spi in 16-bit mode (based on SSD1331 controller).
 *
 * Inits 96x64 RGB OLED display over spi in 16-bit mode (based on SSD1331 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1331_96x64_spi_init16(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * Draws line
 * @param x1 - x position in pixels of start point
 * @param y1 - y position in pixels of start point
 * @param x2 - x position in pixels of end point
 * @param y2 - y position in pixels of end point
 * @param color - color of the line, refer to RGB_COLOR8 macros
 *
 * @note This API can be used only with ssd1331 RGB oled displays
 */
void         ssd1331_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);

/**
 * Copies block in GDRAM to new position
 * @param left column start of block to copy
 * @param top row start of block to copy
 * @param right column end of block to copy
 * @param bottom row end of block to copy
 * @param newLeft new column start
 * @param newTop new row start
 *
 * @note This API can be used only with ssd1331 RGB oled displays
 * @note after copy command is sent, it takes some time from oled
 *       controller to complete operation. So, it is HIGHLY recommended
 *       to wait for reasonable time before send other graphics operations
 *       (for example, use 250us delay). This time is required for
 *       oled display to become ready to accept new commands.
 */
void ssd1331_copyBlock(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t newLeft, uint8_t newTop);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1331_H_
