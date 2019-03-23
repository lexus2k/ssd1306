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
 * @file display.h Drawing in hardware buffer
 */

#ifndef _NANO_DISPLAY_H_
#define _NANO_DISPLAY_H_

#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/canvas.h"
#include "ssd1306_hal/io.h"
#include "nano_gfx_types.h"
#include "lcd/lcd_common.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

enum
{
    CANVAS_MODE_BASIC           = 0x00,
    /** If the flag is specified, text cursor is moved to new line when end of screen is reached */
    CANVAS_TEXT_WRAP            = 0x01,
    /** This flag make bitmaps transparent (Black color) */
    CANVAS_MODE_TRANSPARENT     = 0x02,
    /** If the flag is specified, text cursor is moved to new line when end of canvas is reached */
    CANVAS_TEXT_WRAP_LOCAL      = 0x04,
};

extern uint8_t s_ssd1306_invertByte;

template <class I>
class NanoDisplayBase
{
public:
    /**
     * Creates new empty base display object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    NanoDisplayBase(I& intf): m_intf( intf ) {}

    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy) { };

    /**
     * Returns right-bottom point of the canvas in offset terms.
     * If offset is (0,0), then offsetEnd() will return (width-1,height-1).
     */
    const NanoPoint offsetEnd() const
    {
        return (NanoPoint){ (lcdint_t)(m_w-1), (lcdint_t)(m_h-1) };
    }

    /**
     * Returns rectangle area, covered by canvas in offset terms.
     * If offset is (0,0), then rect() will return ((0,0),(width-1,height-1))
     */
    const NanoRect rect() const
    {
        return { (NanoPoint){0,0}, offsetEnd() };
    }

    /**
     * Returns width of the display in pixels.
     */
    lcduint_t width() { return m_w; }

    /**
     * Returns height of the display in pixels.
     */
    lcduint_t height() { return m_h; }

    /**
     * Sets color for monochrome operations
     * @param color - color to set (refer to RGB_COLOR8 definition)
     */
    void setColor(uint16_t color) { m_color = color; };

    /**
     * All drawing functions start to work in negative mode.
     * Old picture on the display remains unchanged.
     */
    void negativeMode() { s_ssd1306_invertByte = 0xFF; }

    /**
     * All drawing functions start to work in positive (default) mode.
     * Old picture on the display remains unchanged.
     */
    void positiveMode() { s_ssd1306_invertByte = 0x00; }

protected:
    lcduint_t m_w = 0;    ///< width of NanoCanvas area in pixels
    lcduint_t m_h = 0;    ///< height of NanoCanvas area in pixels
    lcduint_t m_p = 0;    ///< number of bits, used by width value: 3 equals to 8 pixels width
    lcdint_t  m_cursorX = 0;  ///< current X cursor position for text output
    lcdint_t  m_cursorY = 0;  ///< current Y cursor position for text output
    uint8_t   m_textMode = 0; ///< Flags for current NanoCanvas mode
    EFontStyle   m_fontStyle; ///< currently active font style
    uint16_t  m_color = 0xFFFF;    ///< current color for monochrome operations

    I& m_intf; ///< communication interface with the display

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For ssd1306 it uses horizontal addressing mode, while for
     * sh1106 the function uses page addressing mode.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - page (top page of the block)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and does not close it.
     *            To close session, please, call endBlock().
     */
//    virtual void startBlock(lcduint_t x, lcduint_t y, lcduint_t w) = 0;

    /**
     * Switches to the start of next RAM page for the block, specified by
     * startBlock().
     * For ssd1306 it does nothing, while for sh1106 the function moves cursor to
     * next page.
     */
//    virtual void nextBlock() = 0;

    /**
     * Closes data send operation to lcd display.
     */
//    virtual void endBlock();
};

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <class I>
class NanoDisplayOps1: public NanoDisplayBase<I>
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = 1;

    using NanoDisplayBase<I>::NanoDisplayBase;

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(lcdint_t x, lcdint_t y);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2);

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - monochrome bitmap data, located in flash
     *
     * @note There are 2 modes: transparent and non-transparent mode, - and 2 colors available: black and white.
     *       In non-transparent mode, when black color is selected, the monochrome image just inverted.
     *       In transparent mode, those pixels of source monochrome image, which are black, do not overwrite pixels
     *       in the screen buffer.
     */
    void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

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
    void drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

    /**
     * Draws bitmap, located in RAM, on the display
     * Each byte represents 8 vertical pixels.
     *
     * ~~~~~~~~~~~~~~~{.cpp}
     * // Draw small rectangle 3x8 at position 10,8
     * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
     * display.drawBuffer1(10, 1, 3, 8, buffer);
     * ~~~~~~~~~~~~~~~
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels (must be divided by 8)
     * @param buffer pointer to data, located in SRAM: each byte represents 8 vertical pixels.
     */
    void drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Draws 8-bit bitmap, located in RAM, on the display
     * Each byte represents one pixel in 2-2-3 format:
     * refer to RGB_COLOR8 to understand RGB scheme, being used.
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in SRAM.
     */
    void drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Draws 16-bit bitmap, located in RAM, on the display
     * Each pixel occupies 2 bytes (5-6-5 format): refer to RGB_COLOR16 to understand RGB scheme, being used.
     *
     * @param xpos horizontal position in pixels
     * @param ypos vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in RAM.
     */
    void drawBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Clears canvas
     */
    void clear();

    /**
     * Fill screen content with specified color
     *
     * @param color color to fill display with
     */
    void fill(uint16_t color);

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string.
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL);

protected:
};

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <class I>
class NanoDisplayOps8: public NanoDisplayBase<I>
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = 8;

    using NanoDisplayBase<I>::NanoDisplayBase;

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(lcdint_t x, lcdint_t y);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2);

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - monochrome bitmap data, located in flash
     *
     * @note There are 2 modes: transparent and non-transparent mode, - and 2 colors available: black and white.
     *       In non-transparent mode, when black color is selected, the monochrome image just inverted.
     *       In transparent mode, those pixels of source monochrome image, which are black, do not overwrite pixels
     *       in the screen buffer.
     */
    void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

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
    void drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

    /**
     * Draws bitmap, located in RAM, on the display
     * Each byte represents 8 vertical pixels.
     *
     * ~~~~~~~~~~~~~~~{.cpp}
     * // Draw small rectangle 3x8 at position 10,8
     * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
     * display.drawBuffer1(10, 1, 3, 8, buffer);
     * ~~~~~~~~~~~~~~~
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels (must be divided by 8)
     * @param buffer pointer to data, located in SRAM: each byte represents 8 vertical pixels.
     */
    void drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Draws 8-bit bitmap, located in RAM, on the display
     * Each byte represents one pixel in 2-2-3 format:
     * refer to RGB_COLOR8 to understand RGB scheme, being used.
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in SRAM.
     */
    void drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Draws 16-bit bitmap, located in RAM, on the display
     * Each pixel occupies 2 bytes (5-6-5 format): refer to RGB_COLOR16 to understand RGB scheme, being used.
     *
     * @param xpos horizontal position in pixels
     * @param ypos vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in RAM.
     */
    void drawBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Clears canvas
     */
    void clear();

    /**
     * Fill screen content with specified color
     *
     * @param color color to fill display with
     */
    void fill(uint16_t color);

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string.
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL);

protected:
};

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <class I>
class NanoDisplayOps16: public NanoDisplayBase<I>
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = 1;

    using NanoDisplayBase<I>::NanoDisplayBase;

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(lcdint_t x, lcdint_t y);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2);

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - monochrome bitmap data, located in flash
     *
     * @note There are 2 modes: transparent and non-transparent mode, - and 2 colors available: black and white.
     *       In non-transparent mode, when black color is selected, the monochrome image just inverted.
     *       In transparent mode, those pixels of source monochrome image, which are black, do not overwrite pixels
     *       in the screen buffer.
     */
    void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

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
    void drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

    /**
     * Draws bitmap, located in RAM, on the display
     * Each byte represents 8 vertical pixels.
     *
     * ~~~~~~~~~~~~~~~{.cpp}
     * // Draw small rectangle 3x8 at position 10,8
     * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
     * display.drawBuffer1(10, 1, 3, 8, buffer);
     * ~~~~~~~~~~~~~~~
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels (must be divided by 8)
     * @param buffer pointer to data, located in SRAM: each byte represents 8 vertical pixels.
     */
    void drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Draws 8-bit bitmap, located in RAM, on the display
     * Each byte represents one pixel in 2-2-3 format:
     * refer to RGB_COLOR8 to understand RGB scheme, being used.
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in SRAM.
     */
    void drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Draws 16-bit bitmap, located in RAM, on the display
     * Each pixel occupies 2 bytes (5-6-5 format): refer to RGB_COLOR16 to understand RGB scheme, being used.
     *
     * @param xpos horizontal position in pixels
     * @param ypos vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in RAM.
     */
    void drawBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer);

    /**
     * Clears canvas
     */
    void clear();

    /**
     * Fill screen content with specified color
     *
     * @param color color to fill display with
     */
    void fill(uint16_t color);

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string.
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL);

protected:
};

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <class O, class I>
class NanoDisplayOps: public O
{
public:
    using O::O;

    using O::putPixel;

    /**
     * Draws pixel on specified position
     * @param p - NanoPoint
     * @note color can be set via setColor()
     */
    void putPixel(const NanoPoint &p);

    /**
     * Draws line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Draws line
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawLine(const NanoRect &rect);

    /**
     * Draws rectangle
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Draws rectangle
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawRect(const NanoRect &rect);

    /**
     * Fills rectangle area
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void fillRect(const NanoRect &rect);

    /**
     * Draws 1-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 1-bit canvas to draw on the screen.
     */
    void drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<1> &canvas);

    /**
     * Draws 8-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 8-bit canvas to draw on the screen.
     */
    void drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<8> &canvas);

    /**
     * Draws 16-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 16-bit canvas to draw on the screen.
     */
    void drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<16> &canvas);

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    size_t write(uint8_t c);

    /**
     * Draws single character to canvas
     * @param c - character code to print
     * @returns 0 if char is not printed
     */
    uint8_t printChar(uint8_t c);

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string, located in flash
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL);

    /**
     * Creates menu object with the provided list of menu items.
     * List of menu items (strings) must exist all until menu object is no longer needed.
     * Selection is set to the first item by default.
     *
     * @param menu pointer to SAppMenu structure
     * @param items array of null-termintated strings (located in SRAM)
     * @param count count of menu items in the array
     */
    void createMenu(SAppMenu *menu, const char **items, uint8_t count);

    /**
     * Shows menu items on the display. If menu items cannot fit the display,
     * the function provides scrolling.
     *
     * @param menu pointer to SAppMenu structure
     */
    void showMenu( SAppMenu *menu);

    /**
     * Updates menu items on the display. That is if selection is changed,
     * the function will update only those areas, affected by the change.
     *
     * @param menu Pointer to SAppMenu structure
     */
    void updateMenu(SAppMenu *menu);

    /**
     * Returns currently selected menu item.
     * First item has zero-index.
     *
     * @param menu pointer to SAppMenu structure
     *
     */
    uint8_t menuSelection(SAppMenu *menu);

    /**
     * Moves selection pointer down by 1 item. If there are no items below,
     * it will set selection pointer to the first item.
     * Use updateMenu() to refresh menu state on the display.
     *
     * @param menu pointer to SAppMenu structure
     */
    void menuDown(SAppMenu *menu);

    /**
     * Moves selection pointer up by 1 item. If selected item is the first one,
     * then selection pointer will set to the last item in menu list.
     * Use updateMenu() to refresh menu state on the display.
     *
     * @param menu pointer to SAppMenu structure
     */
    void menuUp(SAppMenu *menu);

protected:
    /**
     * Initializes interface and display
     */
    virtual void begin() = 0;

    /**
     * closes interface to lcd display
     */
    virtual void end() = 0;
};

#include "ssd1306_1bit.inl"
#include "ssd1306_8bit.inl"
#include "ssd1306_16bit.inl"
#include "ssd1306_common.inl"

/**
 * @}
 */

#endif

