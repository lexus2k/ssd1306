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
 * @file display_base.h Basic display capabilities implementation
 */

#ifndef _DISPLAY_BASE_H_
#define _DISPLAY_BASE_H_

#include "ssd1306_hal/io.h"
#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"

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

/**
 * Class implements basic display operations for the library:
 * It stores reference to communication interafce, display size, etc.
 */
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
};

/**
 * @}
 */

#endif

