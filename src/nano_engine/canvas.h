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

#include "canvas_ops.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <uint8_t BPP>
class NanoCanvasOps: public NanoCanvasOpsInterface
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = BPP;

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
     * Returns right-bottom point of the canvas in offset terms.
     * If offset is (0,0), then offsetEnd() will return (width-1,height-1).
     */
    const NanoPoint offsetEnd() const
    {
        return offset + (NanoPoint){ (lcdint_t)m_w-1, (lcdint_t)m_h-1 };
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
    void putPixel(lcdint_t x, lcdint_t y) override;

    using NanoCanvasOpsInterface::putPixel;

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2) override;

    using NanoCanvasOpsInterface::drawVLine;

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2) override;

    using NanoCanvasOpsInterface::drawHLine;

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    using NanoCanvasOpsInterface::fillRect;

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
    void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) override;

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) override;

    /**
     * Clears canvas
     */
    void clear() override;

protected:
    lcduint_t m_p;        ///< number of bits, used by width value: 3 equals to 8 pixels width
    uint8_t * m_buf;      ///< Canvas data
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

