/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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
 * @file canvas.h Drawing in memory buffer
 */


#ifndef _NANO_CANVAS_OPTS_H_
#define _NANO_CANVAS_OPTS_H_

#include "point.h"
#include "rect.h"
#include "ssd1306_hal/io.h"
#include "ssd1306_hal/Print_internal.h"
#include "nano_gfx_types.h"

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

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
class NanoCanvasOpsInterface: public Print
{
public:
    /** number of bits per single pixel in buffer */
    uint8_t BITS_PER_PIXEL_EX;

    /** Fixed offset for all operation of NanoCanvasOps in pixels */
    NanoPoint offset;

    /////////////////////////////////////////////////////////////////////////////////
    //                       IMPLEMENTATION-SPECIFIC GRAPHICS
    /////////////////////////////////////////////////////////////////////////////////

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    virtual void putPixel(lcdint_t x, lcdint_t y) = 0;

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
    virtual void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) = 0;

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    virtual void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) = 0;

    /**
     * Clears canvas
     */
    virtual void clear() = 0;

    /////////////////////////////////////////////////////////////////////////////////
    //                       IMPLEMENTED, OVERRIDABLE GRAPHICS
    /////////////////////////////////////////////////////////////////////////////////

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    virtual void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    virtual void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2);

    /**
     * Draws line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    virtual void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Draws rectangle
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    virtual void drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    virtual void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    size_t write(uint8_t c) override;

    /////////////////////////////////////////////////////////////////////////////////
    //                       FIXED IMPLEMENTATION GRAPHICS
    /////////////////////////////////////////////////////////////////////////////////

    /**
     * Draws pixel on specified position
     * @param p - NanoPoint
     * @note color can be set via setColor()
     */
    void putPixel(const NanoPoint &p);

    /**
     * Draws line
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawLine(const NanoRect &rect);

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
     * @param ch    pointer to NULL-terminated string.
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL);

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
     * @brief Sets canvas drawing mode
     * Sets canvas drawing mode. The set flags define transparency of output images
     * @param modeFlags - combination of flags: CANVAS_TEXT_WRAP, CANVAS_MODE_TRANSPARENT
     */
    void setMode(uint8_t modeFlags) { m_textMode = modeFlags; };

    /**
     * Sets color for monochrome operations
     * @param color - color to set (refer to RGB_COLOR8 definition)
     */
    void setColor(uint16_t color) { m_color = color; };

    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy) { offset.x = ox; offset.y = oy; };

protected:
    lcduint_t  m_w = 0;     ///< width of NanoCanvas area in pixels
    lcduint_t  m_h = 0;     ///< height of NanoCanvas area in pixels
    lcdint_t   m_cursorX;   ///< current X cursor position for text output
    lcdint_t   m_cursorY;   ///< current Y cursor position for text output
    uint8_t    m_textMode;  ///< Flags for current NanoCanvas mode
    EFontStyle m_fontStyle; ///< currently active font style
    uint16_t   m_color;     ///< current color for monochrome operations
};

/**
 * @}
 */

#endif

