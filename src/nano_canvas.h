/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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

/**
 * NanoCanvas represents objects for drawing in memory buffer
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
        m_p = 3;
        while (w >> (m_p+1)) { m_p++; };
        m_buf = bytes;
        clear();
    };

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @param color - color
     */
    void putPixel(lcdint_t x, lcdint_t y, uint8_t color);

    /**
     * Clears canvas
     */
    void clear();

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     */
    void blt(lcdint_t x, lcdint_t y);

private:
    lcduint_t m_w;
    lcduint_t m_h;
    lcduint_t m_p;
    lcdint_t  m_offsetx;
    lcdint_t  m_offsety;
    uint8_t * m_buf;
};

#endif

