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

/**
 * NanoCanvas8 represents objects for drawing in memory buffer
 * NanoCanvas8 represents each pixel as single byte with RGB bits: RRRGGGBB
 * For details refer to SSD1331 datasheet
 */
class NanoCanvas8
{
public:
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
        m_offsetx = 0;
        m_offsety = 0;
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
    void setOffset(lcdint_t ox, lcdint_t oy) { m_offsetx = ox; m_offsety = oy; };

    /**
     * Gets offset
     * @param ox - pointer to hold X offset in pixels
     * @param oy - pointer to hold Y offset in pixels
     */
    void getOffset(lcdint_t *ox, lcdint_t *oy) { *ox = m_offsetx; *oy = m_offsety; };

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @param color - color
     */
    void putPixel(lcdint_t x, lcdint_t y, uint8_t color);

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
     * Fills rectangle area 
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @param color - color
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint8_t color);

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
    lcdint_t  m_offsetx;
    lcdint_t  m_offsety;
    lcdint_t  m_cursorX;
    lcdint_t  m_cursorY;
    uint8_t   m_textMode;
    uint8_t * m_buf;
    uint8_t   m_color;
};

#endif

