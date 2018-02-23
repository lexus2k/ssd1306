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

#include "nano_canvas.h"
#include "ssd1331_api.h"

#define YADDR8(y) (static_cast<uint16_t>(y) << m_p)
#define BADDR8(b) ((b) << m_p)


void NanoCanvas8::putPixel(lcdint_t x, lcdint_t y, uint8_t color)
{
    x += m_offsetx;
    y += m_offsety;
    if ((x < 0) || (y < 0)) return;
    if ((x >= (lcdint_t)m_w) || (y >= (lcdint_t)m_h)) return;
    m_buf[YADDR8(y) + x] = color;
}

void NanoCanvas8::clear()
{
    uint8_t *buf = m_buf;
    for(uint16_t n = 0; n < YADDR8(m_h); n++)
    {
        *buf = 0;
        buf++;
    }
}

void NanoCanvas8::blt(lcdint_t x, lcdint_t y)
{
    ssd1331_fastDrawBuffer8( x, y, m_w, m_h, m_buf);
}


