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
 * @file canvas.h Drawing in memory buffer
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

    /**
     * Shifts right x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint& operator>>=(const uint8_t bits)
    {
        x >>= bits;
        y >>= bits;
        return *this;
    }

    /**
     * Shifts left x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint& operator<<=(const uint8_t bits)
    {
        x <<= bits;
        y <<= bits;
        return *this;
    }

    /**
     * Adds point.
     * @param p - point values to add to the point. 
     */
    _NanoPoint& operator+=(const _NanoPoint &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    };

    /**
     * Subtracts point.
     * @param p - point values to subtract from the point. 
     */
    _NanoPoint& operator-=(const _NanoPoint &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    };

    /**
     * Subtracts point.
     * @param p - point values to subtract from the point. 
     */
    _NanoPoint operator-(const _NanoPoint &p)
    {
        return { static_cast<lcdint_t>(x - p.x),
                 static_cast<lcdint_t>(y - p.y) };
    };

    /**
     * Adds point.
     * @param p - point values to add to the point. 
     */
    _NanoPoint operator+(const _NanoPoint &p)
    {
        return { static_cast<lcdint_t>(x + p.x),
                 static_cast<lcdint_t>(y + p.y) };
    };

    /**
     * Shifts right x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint operator>>(const uint8_t bits)
    {
        return { static_cast<lcdint_t>(x >> bits),
                 static_cast<lcdint_t>(y >> bits) };
    };

    /**
     * Shifts left x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
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
    /** top-left point of the rectangle area */
    NanoPoint p1;

    /** right-bottom point of the rectangle area */
    NanoPoint p2;

    /**
     * Shifts rectangle area by dx;dy pixels.
     * @param dx - delta on x-axis
     * @param dy - delta on y-axis
     */
    void move(lcdint_t dx, lcdint_t dy)
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

    /**
     * Returns true if specified x position is between left and right borders.
     * @param x - position to check
     */
    bool collisionX(lcdint_t x) const { return (x >= p1.x) && (x <= p2.x); };

    /**
     * Returns true if specified y position is between left and right borders.
     * @param y - position to check
     */
    bool collisionY(lcdint_t y) const { return (y >= p1.y) && (y <= p2.y); };

    /**
     * Returns true if specified point is inside rectangle area.
     * @param p - point to check.
     */
    bool collision(const NanoPoint &p) const { return collisionX(p.x) && collisionY(p.y); };

    /**
     * Returns true if specified point is above rectangle area.
     * @param p - point to check.
     */
    bool above(const NanoPoint &p) const { return (p.y < p1.y); };

    /**
     * Returns true if specified point is below rectangle area.
     * @param p - point to check.
     */
    bool below(const NanoPoint &p) const { return (p.y > p2.y); };

    /**
     * Returns true if specified point is above rectangle area.
     * @param p - point to check.
     */
    _NanoRect operator-(const _NanoPoint &p)
    {
        return { {static_cast<lcdint_t>(p1.x - p.x), static_cast<lcdint_t>(p1.y - p.y) },
                 {static_cast<lcdint_t>(p2.x - p.x), static_cast<lcdint_t>(p2.y - p.y) } };
    };

    /**
     * Add point to all points of rectangle.
     * @param p - point to add.
     */
    _NanoRect operator+(const _NanoPoint &p)
    {
        return { {static_cast<lcdint_t>(p1.x + p.x), static_cast<lcdint_t>(p1.y + p.y) },
                 {static_cast<lcdint_t>(p2.x + p.x), static_cast<lcdint_t>(p2.y + p.y) } };
    };

    /**
     * Subtracts point to all points of rectangle.
     * @param p - point to subtract.
     */
    _NanoRect& operator+=(const _NanoPoint &p)
    {
        p1.x += p.x;
        p1.y += p.y;
        p2.x += p.x;
        p2.y += p.y;
        return *this;
    };

} NanoRect;

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
class NanoCanvas8
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = 8;

    /** Fixed offset for all operation of NanoCanvas8 in pixels */
    NanoPoint offset;

    /**
     * Creates new empty canvas object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    NanoCanvas8()
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
    NanoCanvas8(lcdint_t w, lcdint_t h, uint8_t *bytes)
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
    void begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        m_w = w;
        m_h = h;
        offset.x = 0;
        offset.y = 0;
        m_cursorX = 0;
        m_cursorY = 0;
        m_color = 0xFF; // white color by default
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
     *
     * @note No font style support anymore, sorry for that. Font styles
     *       increase code size.
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch);

    /**
     * Print text at specified position to canvas
     * @param xpos - position in pixels
     * @param y    - position in pixels
     * @param ch - pointer to NULL-terminated string, located in flash
     *
     * @note No font style support anymore, sorry for that. Font styles
     *       increase code size.
     */
    void printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch);

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
class NanoCanvas1
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = 1;

    /** Fixed offset for all operation of NanoCanvas1 in pixels */
    NanoPoint offset;

    /**
     * Creates new empty canvas object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    NanoCanvas1() { };

    /**
     * Creates new canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h / 8.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    NanoCanvas1(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        begin(w, h, bytes);
    };

    /**
     * Initializes canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h / 8.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    void begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        m_w = w;
        m_h = h;
        offset.x = 0;
        offset.y = 0;
        m_cursorX = 0;
        m_cursorY = 0;
        m_color = WHITE;
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

    /*
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
//    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);

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
     *
     * @note No font style support anymore, sorry for that. Font styles
     *       increase code size.
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch);

    /**
     * Print text at specified position to canvas
     * @param xpos - position in pixels
     * @param y    - position in pixels
     * @param ch - pointer to NULL-terminated string, located in flash
     *
     * @note No font style support anymore, sorry for that. Font styles
     *       increase code size.
     */
    void printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch);

    /**
     * @brief Sets canvas drawing mode
     * Sets canvas drawing mode. The set flags define transparency of output images
     * @param modeFlags - combination of flags: CANVAS_TEXT_WRAP, CANVAS_MODE_TRANSPARENT
     */
    void setMode(uint8_t modeFlags) { m_textMode = modeFlags; };

    /**
     * Sets color for monochrome operations
     * @param color - color to set (refer to RGB_COLOR8 definition)
     *
     * @note 0 means black color
     *       1-255 means white color
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
class NanoCanvas16
{
public:
    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = 16;

    /** Fixed offset for all operation of NanoCanvas8 in pixels */
    NanoPoint offset;

    /**
     * Creates new empty canvas object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    NanoCanvas16()
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
    NanoCanvas16(lcdint_t w, lcdint_t h, uint8_t *bytes)
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
    void begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        m_w = w;
        m_h = h;
        offset.x = 0;
        offset.y = 0;
        m_cursorX = 0;
        m_cursorY = 0;
        m_color = 0xFFFF; // white color by default
        m_textMode = 0;
        m_p = 3;
        while (w >> (m_p+1)) { m_p++; };
        m_p++;
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
     *
     * @note No font style support anymore, sorry for that. Font styles
     *       increase code size.
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch);

    /**
     * Print text at specified position to canvas
     * @param xpos - position in pixels
     * @param y    - position in pixels
     * @param ch - pointer to NULL-terminated string, located in flash
     *
     * @note No font style support anymore, sorry for that. Font styles
     *       increase code size.
     */
    void printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch);

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
    uint16_t   m_color;
};


#endif

