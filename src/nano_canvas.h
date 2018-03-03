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
 * @file nano_canvas.h Drawing in memory buffer
 */


#ifndef _NANO_CANVAS_H_
#define _NANO_CANVAS_H_

#include "hal/io.h"
#include "nano_gfx_types.h"

enum
{
    CANVAS_TEXT_WRAP = 1,
    CANVAS_MODE_TRANSPARENT = 2,
};


/** Describes point */
typedef struct _NanoPoint
{
    /** x position in pixels */
    lcdint_t x;
    /** y position in pixels */
    lcdint_t y;

    /**
     * Initializes NanoPoint with specified values
     * @param px - x position
     * @param py - y position
     */
    void setPoint(lcdint_t px, lcdint_t py) { x=px; y=py; };

    _NanoPoint& operator>>=(const uint8_t bits)
    {
        x >>= bits;
        y >>= bits;
        return *this;
    }

    _NanoPoint& operator<<=(const uint8_t bits)
    {
        x <<= bits;
        y <<= bits;
        return *this;
    }

    _NanoPoint& operator+=(const _NanoPoint &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    };

    _NanoPoint& operator-=(const _NanoPoint &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    };

    _NanoPoint operator-(const _NanoPoint &p)
    {
        return { static_cast<lcdint_t>(x - p.x),
                 static_cast<lcdint_t>(y - p.y) };
    };

    _NanoPoint operator+(const _NanoPoint &p)
    {
        return { static_cast<lcdint_t>(x + p.x),
                 static_cast<lcdint_t>(y + p.y) };
    };

    _NanoPoint operator>>(const uint8_t bits)
    {
        return { static_cast<lcdint_t>(x >> bits),
                 static_cast<lcdint_t>(y >> bits) };
    };

    _NanoPoint operator<<(const uint8_t bits)
    {
        return { static_cast<lcdint_t>(x << bits),
                 static_cast<lcdint_t>(y << bits) };
    };

} NanoPoint;

/**
 * NanoRect structure describes rectangle area.
 */
typedef struct _NanoRect
{
    NanoPoint p1;

    NanoPoint p2;

    /**
     * Shifts rectangle area by dx;dy pixels.
     * @param dx - delta on x-axis
     * @param dy - delta on y-axis
     */
    void add(lcdint_t dx, lcdint_t dy)
    {
        p1.x += dx; p2.x += dx;
        p1.y += dy; p2.y += dy;
    }

    /**
     * Shifts rectangle area by dx pixels.
     * @param dx - delta on x-axis
     */
    void addH(lcdint_t dx)
    {
        p1.x += dx; p2.x += dx;
    };

    /**
     * Shifts rectangle area by dy pixels.
     * @param dy - delta on y-axis
     */
    void addV(lcdint_t dy)
    {
        p1.y += dy;
        p2.y += dy;
    };

    /**
     * Initializes NanoRect with specified values
     * @param l - left position
     * @param t - top position
     * @param r - right position
     * @param b - bottom position
     */
    void setRect(lcdint_t l, lcdint_t t, lcdint_t r, lcdint_t b)
    {
        p1.x = l; p1.y = t;
        p2.x = r; p2.y = b;
    };

    bool hasX(lcdint_t x) const { return (x >= p1.x) && (x <= p2.x); };

    bool hasY(lcdint_t y) const { return (y >= p1.y) && (y <= p2.y); };

    bool has(const NanoPoint &p) const { return hasX(p.x) && hasY(p.y); };

    bool above(const NanoPoint &p) const { return (p.y < p1.y); };

    bool below(const NanoPoint &p) const { return (p.y > p2.y); };

    _NanoRect operator-(const _NanoPoint &p)
    {
        return { {static_cast<lcdint_t>(p1.x - p.x), static_cast<lcdint_t>(p1.y - p.y) },
                 {static_cast<lcdint_t>(p2.x - p.x), static_cast<lcdint_t>(p2.y - p.y) } };
    };

    _NanoRect operator+(const _NanoPoint &p)
    {
        return { {static_cast<lcdint_t>(p1.x + p.x), static_cast<lcdint_t>(p1.y + p.y) },
                 {static_cast<lcdint_t>(p2.x + p.x), static_cast<lcdint_t>(p2.y + p.y) } };
    };

    _NanoRect& operator+=(const _NanoPoint &p)
    {
        p1.x += p.x;
        p1.y += p.y;
        p2.x += p.x;
        p2.y += p.y;
        return *this;
    };

} NanoRect;

/**
 * NanoCanvas8 represents objects for drawing in memory buffer
 * NanoCanvas8 represents each pixel as single byte with RGB bits: RRRGGGBB
 * For details refer to SSD1331 datasheet
 */
class NanoCanvas8
{
public:
    /** Fixed offset for all operation of NanoCanvas8 in pixels */
    NanoPoint offset;

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
    NanoCanvas8(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        m_w = w;
        m_h = h;
        offset.x = 0;
        offset.y = 0;
        m_cursorX = 0;
        m_cursorY = 0;
        m_color = 0xFF;
        m_textMode = 0;
        m_p = 3;
        while (w >> (m_p+1)) { m_p++; };
        m_buf = bytes;
        clear();
    };

    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy) { offset.x = ox; offset.y = oy; };

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @param color - color
     */
    void putPixel(lcdint_t x, lcdint_t y, uint8_t color);

    /**
     * Draws pixel on specified position
     * @param p - NanoPoint
     * @param color - color
     */
    void putPixel(const NanoPoint &p, uint8_t color);

    /**
     * Draws horizontal or vertical line 
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @param color - color
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2, uint8_t color);

    /**
     * Draws horizontal or vertical line 
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param color - color
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, uint8_t color);

    /**
     * Draws rectangle 
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @param color - color
     */
    void drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint8_t color);

    /**
     * Draws rectangle 
     * @param rect - structure, describing rectangle area
     * @param color - color
     */
    void drawRect(const NanoRect &rect, uint8_t color);

    /**
     * Fills rectangle area 
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @param color - color
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint8_t color);

    /**
     * Fills rectangle area
     * @param rect - structure, describing rectangle area
     * @param color - color
     */
    void fillRect(const NanoRect &rect, uint8_t color);

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - monochrome bitmap data, located in flash
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
     * Clears canvas
     */
    void clear();

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    void write(uint8_t c);

    /**
     * Draws single character to canvas
     * @param c - character code to print
     */
    void printChar(uint8_t c);

    /**
     * Print text at specified position to canvas
     * @param xpos - position in pixels
     * @param y    - position in pixels
     * @param ch - pointer to NULL-terminated string.
     * @param style - font style (EFontStyle), normal by default
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style);

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
    void setColor(uint8_t color) { m_color = color; };

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     */
    void blt(lcdint_t x, lcdint_t y);

    /**
     * Draws canvas on the LCD display using offset values.
     */
    void blt();

private:
    lcduint_t m_w;
    lcduint_t m_h;
    lcduint_t m_p;
    lcdint_t  m_cursorX;
    lcdint_t  m_cursorY;
    uint8_t   m_textMode;
    uint8_t * m_buf;
    uint8_t   m_color;
};

#endif

