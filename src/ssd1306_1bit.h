/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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
 * @file ssd1306_1bit.h SSD1306 basic draw functions
 */


#ifndef _SSD1306_1BIT_H_
#define _SSD1306_1BIT_H_

#include "nano_gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//                 DIRECT GRAPH FUNCTIONS
///////////////////////////////////////////////////////////////////////

/**
 * @defgroup LCD_1BIT_GRAPHICS DIRECT DRAW: 1-bit graphic functions for ssd1306 compatible mode.
 * @{
 * @brief LCD direct draw functions for all display types: color and monochrome.
 *
 * @details LCD direct draw functions are applicable for all display types. These functions will work
 *        both for monochrome and 8-bit/16-bit color OLED displays. You need remember, that for RGB
 *        oled displays these functions work only in special ssd1306 compatible mode. If you're going to
 *        combine NanoEngine capabilities with these functions, don't forget to switch addressing
 *        mode via ssd1306_setMode().
 *        Direct draw functions draw directly in GDRAM and do not use any double-buffering.
 */

/**
 * Fills screen with pattern byte
 */
void         ssd1306_fillScreen(uint8_t fill_Data);

/**
 * Fills screen with zero-byte
 */
void         ssd1306_clearScreen(void);

/**
 * All drawing functions start to work in negative mode.
 * Old picture on the display remains unchanged.
 */
void         ssd1306_negativeMode(void);

/**
 * All drawing functions start to work in positive (default) mode.
 * Old picture on the display remains unchanged.
 */
void         ssd1306_positiveMode(void);

/**
 * Prints text to screen using fixed font.
 * @param xpos - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle), normal by default.
 * @returns number of chars in string
 * @see ssd1306_setFixedFont
 * @warning ssd1306_printFixed() can output chars at fixed y positions: 0, 8, 16, 24, 32, etc.
 *          If you specify [10,18], ssd1306_printFixed() will output text starting at [10,16] position.
 * @warning Be careful with you flash space! Do not mix too many different functions in single sketch.
 *          ssd1306_printFixedN() uses much flash: ~396 bytes, ssd1306_printFixed() needs 388 bytes.
 *          Placing both of these functions to your sketch will consume almost 1KiB.
 */
uint8_t     ssd1306_printFixed(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
uint8_t ssd1306_printFixed_oldStyle(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style);
#endif

/**
 * Prints text to screen using double size fixed font.
 * @param xpos - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle), normal by default.
 * @returns number of chars in string
 * @see ssd1306_setFixedFont
 * @warning ssd1306_printFixed2x() can output chars at fixed y positions: 0, 8, 16, 24, 32, etc.
 *          If you specify [10,18], ssd1306_printFixed2x() will output text starting at [10,16] position.
 * @warning Be careful with you flash space! Do not mix too many different functions in single sketch.
 *          ssd1306_printFixedN() uses much flash: ~474 bytes, ssd1306_printFixed() needs 388 bytes.
 *          Placing both of these functions to your sketch will consume almost 1KiB.
 * @deprecated Use ssd1306_printFixedN() instead.
 */
uint8_t     ssd1306_printFixed2x(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style) __attribute__ ((deprecated));

/**
 * Prints text to screen using size fixed font, scaled by factor value. <br>
 * Factor value 0 gives regular font size (6x8 for example) <br>
 * Factor value 1 gives double font size (12x16 if 6x8 font is used) <br>
 * Factor value 2 gives fourth font size (24x32 if 6x8 font is used) <br>
 * Factor value 3 gives eighth font size (48x64 if 6x8 font is used) <br>
 * @param xpos - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle), normal by default.
 * @param factor - 0, 1, 2, 3.
 * @returns number of chars in string
 * @see ssd1306_setFixedFont
 * @warning ssd1306_printFixed2x() can output chars at fixed y positions: 0, 8, 16, 24, 32, etc.
 *          If you specify [10,18], ssd1306_printFixed2x() will output text starting at [10,16] position.
 * @warning Be careful with you flash space! Do not mix too many different functions in single sketch.
 *          ssd1306_printFixedN() uses much flash: ~474 bytes, ssd1306_printFixed() needs 388 bytes.
 *          Placing both of these functions to your sketch will consume almost 1KiB.
 */
uint8_t     ssd1306_printFixedN(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style, uint8_t factor);

/**
 * @brief Prints single character to display at current cursor position
 *
 * Prints single character to display at current cursor position
 * @param ch - character to print to the display. 'LF' and 'CR' are skipped
 * @return returns number of printed characters.
 */
size_t      ssd1306_write(uint8_t ch);

/**
 * @brief Prints null-terminated string to display at current cursor position
 *
 * Prints null-terminated string to display at current cursor position
 * @param ch - string to print to the display. 'LF' and 'CR' are skipped
 * @return returns number of printed characters.
 */
size_t      ssd1306_print(const char ch[]);

/**
 * Prints text to screen using font 6x8.
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle), normal by default.
 * @returns number of chars in string
 * @deprecated Use ssd1306_printFixed() instead.
 */
uint8_t      ssd1306_charF6x8(uint8_t x, uint8_t y,
                              const char ch[],
                              EFontStyle style
#ifdef __cplusplus
                              = STYLE_NORMAL
#endif
                             ) __attribute__ ((deprecated));

/**
 * Prints text to screen using double size font 12x16.
 * @param xpos - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle).
 * @returns number of chars in string
 * @deprecated Use ssd1306_drawFixedN() instead.
 */
uint8_t      ssd1306_charF12x16(uint8_t xpos,
                                uint8_t y,
                                const char ch[],
                                EFontStyle style) __attribute__ ((deprecated));


/**
 * Prints text to screen using set font.
 * If real text ends before right boundary,
 * the remaining part on the display will be erased till right
 * boundary.
 * @param left - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle), normal by default.
 * @param right - right boundary of the text to output
 * @returns number of chars in string
 * @deprecated This function is removed as superflouse.
 */
uint8_t      ssd1306_charF6x8_eol(uint8_t left,
                                  uint8_t y,
                                  const char ch[],
                                  EFontStyle style,
                                  uint8_t right) __attribute__ ((deprecated));

/**
 * Put single pixel on the LCD.
 *
 * @warning Please, take into account that there is no way
 * to read data from ssd1306, thus since each byte contains
 * 8 pixels, all other pixels in the same byte will be cleared
 * on the display. Use ssd1306_putPixels() instead.
 * If you need to have buffered output, please, refer to NanoCanvas.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 */
void         ssd1306_putPixel(uint8_t x, uint8_t y);

/**
 * Puts eight vertical pixels on the LCD at once.
 *
 * ~~~~~~~~~~~~~~~{.c}
 * // Draw 8 vertical pixels, starting at [10,16] position
 * ssd1306_putPixels(10,2,0xFF);
 * // Draw 4 vertical pixels, starting at [32,28] position
 * ssd1306_putPixels(32,3,0x0F);
 * ~~~~~~~~~~~~~~~
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position pixels. Should be multiply of 8.
 * @param pixels - bit-pixels to draw on display
 */
void         ssd1306_putPixels(uint8_t x, uint8_t y, uint8_t pixels);

/**
 * Draws rectangle
 * @param x1 - left boundary in pixel units
 * @param y1 - top boundary in pixel units
 * @param x2 - right boundary in pixel units
 * @param y2 - bottom boundary int pixel units
 */
void         ssd1306_drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

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
void ssd1306_fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

/**
 * Draws line
 * @param x1 - x position in pixels of start point
 * @param y1 - y position in pixels of start point
 * @param x2 - x position in pixels of end point
 * @param y2 - y position in pixels of end point
 *
 * @warning Remember that this function draws line directly in GDRAM of oled controller.
 *          Since there is no way to detect pixels already being displayed, some pixels
 *          can be overwritten by black color. If you use RGB oled, based on ssd1331 controller,
 *          use ssd1331_drawLine() function.
 */
void         ssd1306_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/**
 * Draws horizontal line
 * @param x1 - left boundary in pixels
 * @param y1 - position Y in pixels
 * @param x2 - right boundary in pixels
 */
void         ssd1306_drawHLine(uint8_t x1, uint8_t y1, uint8_t x2);

/**
 * Draws vertical line
 * @param x1 - position X in pixels
 * @param y1 - top boundary in pixels
 * @param y2 - bottom boundary in pixels
 */
void         ssd1306_drawVLine(uint8_t x1, uint8_t y1, uint8_t y2);

/**
 * Draws bitmap, located in SRAM, on the display
 * Each byte represents 8 vertical pixels.
 *
 * ~~~~~~~~~~~~~~~{.c}
 * // Draw small rectangle 3x8 at position 10,8
 * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
 * ssd1306_drawBuffer(10, 1, 3, 8, buffer);
 * ~~~~~~~~~~~~~~~
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in SRAM: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawBuffer(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);

/**
 * Draws bitmap, located in SRAM, on the display
 * Each byte represents 2 horizontal grayscale pixels.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in SRAM: each byte represents 2 horizontal pixels.
 */
void         ssd1306_drawBuffer1_4(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);


/**
 * Draws bitmap, located in SRAM, on the display
 * Each byte represents 8 vertical pixels.
 *
 * ~~~~~~~~~~~~~~~{.c}
 * // Draw small rectangle 3x8 at position 10,8
 * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
 * ssd1306_drawBuffer(10, 1, 3, 8, buffer);
 * ~~~~~~~~~~~~~~~
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels (must be devided by 8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in SRAM: each byte represents 8 vertical pixels.
 *
 * @note ssd1306_drawBufferFast() doesn't support negative draw mode. Use ssd1306_drawBuffer() instead.
 */
void         ssd1306_drawBufferFast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf);

/**
 * @copydoc ssd1306_drawBuffer
 */
#define      ssd1306_drawCanvas(x, y, w, h, buf)   ssd1306_drawBuffer(x, y, w, h, buf)

/**
 * Draws bitmap, located in Flash, on the display
 * The bitmap should be in native ssd1306 format
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);

/**
 * Draws bitmap, located in Flash, on the display
 * The bitmap should be in XBMP format
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawXBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);

/**
 * Draw bitmap, located in Flash, on the display
 * The bitmap should be in in grayscale 4-bit format:
 * ROW 1: 222211114444333366665555...
 * ROW 2: 222211114444333366665555...
 * ...
 * ROW N: 222211114444333366665555...
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in Flash.
 */
void         ssd1306_drawBitmap1_4(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);

/**
 * Draws bitmap, located in Flash, on the display
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf);

/**
 * Fills block with black pixels
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of block in pixels
 * @param h - height of block in pixels (must be divided by 8)
 * @note usually this method is used to erase bitmap on the screen.
 */
void         ssd1306_clearBlock(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/**
 * Draws bitmap, located in Flash, on the display. This sprite must have wx8 size
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width in pixels
 * @param sprite - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawSpriteEx(uint8_t x, uint8_t y, uint8_t w, const uint8_t *sprite);

/**
 * Draws sprite on the display. Position can be changed by
 * updating x and y fields of SPRITE structure.
 * @param sprite - pointer to SPRITE structure
 */
void         ssd1306_drawSprite(SPRITE *sprite);

/**
 * Clears sprite from the display leaving black rectangle.
 * @param sprite - pointer to SPRITE structure
 */
void         ssd1306_eraseSprite(SPRITE *sprite);

/**
 * Clears some sprite parts in old position on the display.
 * @param sprite - pointer to SPRITE structure
 */
void         ssd1306_eraseTrace(SPRITE *sprite);

/**
 * Creates sprite object. Sprite height is fixed to 8 pixels
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of sprite in pixels
 * @param data - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 * @return SPRITE structure
 */
SPRITE       ssd1306_createSprite(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data);

/**
 * Replaces image of the sprite with different data. The width must be the same as
 * the width of original sprite image
 * @param sprite - pointer to SPRITE structure
 * @param data - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_replaceSprite(SPRITE *sprite, const uint8_t *data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _SSD1306_1BIT_H_
