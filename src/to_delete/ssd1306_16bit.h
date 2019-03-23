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
 * @file ssd1306_16bit.h 16-bit specific draw functions
 */

#ifndef _SSD1306_16BIT_H_
#define _SSD1306_16BIT_H_

#include "nano_gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//                 DIRECT GRAPH FUNCTIONS
///////////////////////////////////////////////////////////////////////

/**
 * @defgroup LCD_16BIT_GRAPHICS DIRECT DRAW: 16-bit API functions only for color displays
 * @{
 *
 * @brief LCD direct draw functions only for color display.
 *
 * @details LCD direct draw functions are applicable for color display types. These functions will NOT work
 *        in ssd1306 compatible mode. Use ssd1306_setMode() function to change display mode to NORMAL.
 *        You can combine combine NanoEngine capabilities with these functions.
 *        Direct draw functions draw directly in GDRAM and do not use any double-buffering.
 */

/**
 * @brief Sets default color.
 *
 * Sets default color for monochrome operations.
 * This function supports only 16-bit RGB mode.
 * To work with RGB colors in 8-bit mode, please refer to ssd1306_setRgbColor8() function
 * and RGB_COLOR8 macros.
 * @param r - red in 0-255 range.
 * @param g - green in 0-255 range.
 * @param b - blue in 0-255 range.
 */
void ssd1306_setRgbColor16(uint8_t r, uint8_t g, uint8_t b);

/**
 * Draws 16-bit bitmap, located in SRAM, on the display
 * Each byte represents separate pixel: refer to RGB_COLOR16 to understand RGB scheme, being used.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels
 * @param data - pointer to data, located in SRAM.
 */
void ssd1306_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static inline void ssd1331_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferFast16(x, y, w, h, data);
}
#endif

/**
 * Draws 16-bit bitmap, located in SRAM, on the display, taking into account pitch parameter.
 * Each byte represents separate pixel: refer to RGB_COLOR16 to understand RGB scheme, being used.
 * pitch parameter specifies, length of single line in bytes.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels
 * @param pitch length of bitmap buffer line in bytes
 * @param data - pointer to data, located in SRAM.
 */
void ssd1306_drawBufferEx16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data);

/**
 * Draws 1-bit bitmap, located in SRAM, on the display
 * Each bit represents separate pixel: refer to ssd1306 datasheet for more information.
 *
 * @param xpos horizontal position in pixels
 * @param ypos vertical position in pixels
 * @param w width of bitmap in pixels
 * @param h height of bitmap in pixels
 * @param bitmap pointer to data, located in SRAM.
 */
void ssd1306_drawMonoBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

/**
 * Fills screen with pattern byte
 *
 * @param fill_Data pattern color to fill screen with
 */
void ssd1306_fillScreen16(uint16_t fill_Data);

/**
 * Fills screen with zero-byte
 */
void ssd1306_clearScreen16(void);

/**
 * Puts single color point directly in OLED display GDRAM.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_putPixel16(lcdint_t x, lcdint_t y);

/**
 * Puts single color point directly in OLED display GDRAM.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param color color in 16-bit format: 5-6-5
 */
void ssd1306_putColorPixel16(lcdint_t x, lcdint_t y, uint16_t color);

/**
 * Draw vertical line directly in OLED display GDRAM.
 *
 * @param x1 - horizontal position in pixels
 * @param y1 - top vertical position in pixels
 * @param y2 - bottom vertical position in pixels
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_drawVLine16(lcdint_t x1, lcdint_t y1, lcdint_t y2);

/**
 * Draw horizontal line directly in OLED display GDRAM.
 *
 * @param x1 - left position in pixels
 * @param y1 - vertical vertical position in pixels
 * @param x2 - right position in pixels
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_drawHLine16(lcdint_t x1, lcdint_t y1, lcdint_t x2);

/**
 * Draw line directly in OLED display GDRAM.
 * This is software implementation. Some OLED controllers have hardware implementation.
 * Refer to datasheet.
 *
 * @param x1 - start horizontal position in pixels
 * @param y1 - start vertical position in pixels
 * @param x2 - end horizontal position in pixels
 * @param y2 - end vertical position in pixels
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_drawLine16(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

/**
 * Draw rectangle directly in OLED display GDRAM.
 * This is software implementation. Some OLED controllers have hardware implementation.
 * Refer to datasheet.
 *
 * @param x1 - start horizontal position in pixels
 * @param y1 - start vertical position in pixels
 * @param x2 - end horizontal position in pixels
 * @param y2 - end vertical position in pixels
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_drawRect16(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

/**
 * Fill rectangle directly in OLED display GDRAM.
 * This is software implementation. Some OLED controllers have hardware implementation.
 * Refer to datasheet.
 *
 * @param x1 - start horizontal position in pixels
 * @param y1 - start vertical position in pixels
 * @param x2 - end horizontal position in pixels
 * @param y2 - end vertical position in pixels
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_fillRect16(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

/**
 * Draw monochrome bitmap, located in Flash, directly to OLED display GDRAM.
 * The bitmap should be in ssd1306 format (each byte represents 8 vertical pixels)
 *
 * @param xpos start horizontal position in pixels
 * @param ypos start vertical position in pixels
 * @param w bitmap width in pixels
 * @param h bitmap height in pixels
 * @param bitmap pointer to Flash data, containing monochrome bitmap.
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_drawMonoBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

/**
 * Draw 16-bit color bitmap, located in Flash, directly to OLED display GDRAM.
 * Each pixel of the bitmap is expected in 5-6-5 format.
 *
 * @param xpos start horizontal position in pixels
 * @param ypos start vertical position in pixels
 * @param w bitmap width in pixels
 * @param h bitmap height in pixels
 * @param bitmap pointer to Flash data, containing 16-bit color bitmap.
 */
void ssd1306_drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

/**
 * Clears block, filling it with black pixels, directly in OLED display GDRAM.
 *
 * @param x start horizontal position in pixels
 * @param y start vertical position in pixels
 * @param w block width in pixels
 * @param h block height in pixels
 */
void ssd1306_clearBlock16(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/**
 * Set cursor position for text functions
 *
 * @param x horizontal position in pixels.
 * @param y vertical position in pixels.
 */
void ssd1306_setCursor16(lcduint_t x, lcduint_t y);

/**
 * Draws single character to canvas. Cursor position is defined
 * by ssd1306_setCursor16(). Do not changes cursor position
 *
 * @param c - character code to print
 *
 * @note set color with ssd1306_setColor() function.
 */
void ssd1306_printChar16(uint8_t c);

/**
 * @brief Prints single character to display at current cursor position
 *
 * Prints single character to display at current cursor position.
 * Cursor position can be set by ssd1306_setCursor16().
 *
 * @param ch - character to print to the display. 'LF' and 'CR' are skipped
 * @return returns number of printed characters.
 */
size_t ssd1306_write16(uint8_t ch);

/**
 * @brief Prints null-terminated string to display at current cursor position
 *
 * Prints null-terminated string to display at current cursor position
 * Cursor position can be set by ssd1306_setCursor16().
 *
 * @param ch - string to print to the display. 'LF' and 'CR' are skipped
 * @return returns number of printed characters.
 */
size_t ssd1306_print16(const char ch[]);

/**
 * Prints text to screen using fixed font.
 * @param x horizontal position in pixels
 * @param y vertical position in pixels
 * @param ch NULL-terminated string to print
 * @param style font style (EFontStyle), normal by default (not implemented).
 * @returns number of chars in string
 *
 * @see ssd1306_setFixedFont
 * @note set color with ssd1306_setColor() function.
 */
uint8_t ssd1306_printFixed16(lcdint_t x, lcdint_t y, const char *ch, EFontStyle style);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _SSD1306_16BIT_H_
