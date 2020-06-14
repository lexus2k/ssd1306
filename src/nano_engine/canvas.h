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
 * @file canvas.h Drawing in memory buffer
 */


#ifndef _NANO_CANVAS_H_
#define _NANO_CANVAS_H_

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
template <uint8_t BPP>
class NanoCanvasOps: public Print
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = BPP;

    /** Fixed offset for all operation of NanoCanvasOps in pixels */
    NanoPoint offset = { 0, 0 };

    /**
     * Creates new empty canvas object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    NanoCanvasOps()
    {
    }

    /**
     * Creates new canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    NanoCanvasOps(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        begin(w, h, bytes);
    }

    /**
     * Initializes canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    void begin(lcdint_t w, lcdint_t h, uint8_t *bytes);

    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy) { offset.x = ox; offset.y = oy; };

    /**
     * Returns right-bottom point of the canvas in offset terms.
     * If offset is (0,0), then offsetEnd() will return (width-1,height-1).
     */
    const NanoPoint offsetEnd() const
    {
        return offset + (NanoPoint){ (lcdint_t)(m_w-1), (lcdint_t)(m_h-1) };
    }

    /**
     * Returns rectangle area, covered by canvas in offset terms.
     * If offset is (0,0), then rect() will return ((0,0),(width-1,height-1))
     */
    const NanoRect rect() const
    {
        return { offset, offsetEnd() };
    }

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(lcdint_t x, lcdint_t y);

    /**
     * Draws pixel on specified position
     * @param p - NanoPoint
     * @note color can be set via setColor()
     */
    void putPixel(const NanoPoint &p);

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
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Fills rectangle area
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void fillRect(const NanoRect &rect);

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * The bitmap is expected in Native ssd1306 controller format.
     *
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
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * The bitmap is expected in XBMP format.
     *
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
    void drawXBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

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
     * Clears canvas
     */
    void clear();

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    size_t write(uint8_t c) override;

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

protected:
    lcduint_t m_w;    ///< width of NanoCanvas area in pixels
    lcduint_t m_h;    ///< height of NanoCanvas area in pixels
    lcduint_t m_p;    ///< number of bits, used by width value: 3 equals to 8 pixels width
    lcdint_t  m_cursorX;  ///< current X cursor position for text output
    lcdint_t  m_cursorY;  ///< current Y cursor position for text output
    uint8_t   m_textMode; ///< Flags for current NanoCanvas mode
    EFontStyle   m_fontStyle; ///< currently active font style
    uint8_t * m_buf;      ///< Canvas data
    uint16_t  m_color;    ///< current color for monochrome operations
};

/**
 * Base class for all NanoCanvas childs
 */
template <uint8_t BPP>
class NanoCanvasBase: public NanoCanvasOps<BPP>
{
public:
    using NanoCanvasOps<BPP>::NanoCanvasOps;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     */
    virtual void blt(lcdint_t x, lcdint_t y) = 0;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    virtual void blt() = 0;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    virtual void blt(const NanoRect &rect) = 0;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

enum
{
    BLACK = 0x00,  ///< Black color
    WHITE = 0xFF,  ///< White color
};

/**
 * NanoCanvas1 represents objects for drawing in memory buffer
 * NanoCanvas1 represents each pixel as single bit: 0/1
 * For details refer to SSD1306 datasheet
 */
class NanoCanvas1: public NanoCanvasBase<1>
{
public:
    using NanoCanvasBase::NanoCanvasBase;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void blt(lcdint_t x, lcdint_t y) override;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt() override;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    void blt(const NanoRect &rect) override;
};

/**
 * NanoCanvas1_8 represents objects for drawing in memory buffer
 * NanoCanvas1_8 represents each pixel as single bit: 0/1
 * Unlike NanoCanvas1, it works with RBG color displays in normal mode.
 */
class NanoCanvas1_8: public NanoCanvasBase<1>
{
public:
    using NanoCanvasBase::NanoCanvasBase;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void blt(lcdint_t x, lcdint_t y) override;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt() override;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    void blt(const NanoRect &rect) override;
};

/**
 * NanoCanvas1_16 represents objects for drawing in memory buffer
 * NanoCanvas1_16 represents each pixel as single bit: 0/1
 * Unlike NanoCanvas1, it works with RBG color displays in normal mode.
 */
class NanoCanvas1_16: public NanoCanvasBase<1>
{
public:
    using NanoCanvasBase::NanoCanvasBase;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void blt(lcdint_t x, lcdint_t y) override;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt() override;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    void blt(const NanoRect &rect) override;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             4-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * NanoCanvas1_4 represents objects for drawing in memory buffer
 * NanoCanvas1_4 represents each pixel as 4-bits in GRAYscale: 11112222
 * For details refer to ssd1327/ssd1325 datasheet
 */
class NanoCanvas1_4: public NanoCanvasBase<4>
{
public:
    using NanoCanvasBase::NanoCanvasBase;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void blt(lcdint_t x, lcdint_t y) override;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt() override;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    void blt(const NanoRect &rect) override;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * NanoCanvas8 represents objects for drawing in memory buffer
 * NanoCanvas8 represents each pixel as single byte with RGB bits: RRRGGGBB
 * For details refer to SSD1331 datasheet
 */
class NanoCanvas8: public NanoCanvasBase<8>
{
public:
    using NanoCanvasBase::NanoCanvasBase;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void blt(lcdint_t x, lcdint_t y) override;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt() override;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    void blt(const NanoRect &rect) override;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * NanoCanvas16 represents objects for drawing in memory buffer
 * NanoCanvas16 represents each pixel as 2-bytes with RGB bits: RRRRRGGG-GGGBBBBB
 * For details refer to SSD1351 datasheet
 */
class NanoCanvas16: public NanoCanvasBase<16>
{
public:
    using NanoCanvasBase::NanoCanvasBase;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void blt(lcdint_t x, lcdint_t y) override;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt() override;

    /**
     * Draws only part of canvas on the LCD display.
     * This method uses Canvas offset field as top-left point of whole canvas
     * content. First point of specified rectangle defines the actual top-left
     * point on the screen to be refreshed.
     * For example, `blt({{8,0},{15,7}});` will copy canvas area {8,0}-{15,7}
     * to screen starting at {8,0} if canvas offset is {0,0}.
     * If canvas offset is {12,3}, then canvas area {8,0}-{15,7} will be copied
     * to screen at position {20,3}.
     * @param rect rectagle describing part of canvas to move to display.
     */
    void blt(const NanoRect &rect) override;
};

/**
 * @}
 */

#endif

