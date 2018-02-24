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

#include "nano_canvas.h"
#include "lcd/lcd_common.h"
#include "ssd1331_api.h"

#define YADDR8(y) (static_cast<uint16_t>(y) << m_p)
#define BADDR8(b) ((b) << m_p)
#define swap_data(a, b ,type)  { type t = a; a = b; b = t; }

extern SFixedFontInfo s_fixedFont;

void NanoCanvas8::putPixel(lcdint_t x, lcdint_t y, uint8_t color)
{
    x -= m_offsetx;
    y -= m_offsety;
    if ((x < 0) || (y < 0)) return;
    if ((x >= (lcdint_t)m_w) || (y >= (lcdint_t)m_h)) return;
    m_buf[YADDR8(y) + x] = color;
}

void NanoCanvas8::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2, uint8_t color)
{
    x1 -= m_offsetx;
    y1 -= m_offsety;
    y2 -= m_offsety;
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if ((x1 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    y1 = max(y1,0);
    y2 = min(y2,(lcdint_t)m_h-1);
    uint8_t *buf = m_buf + YADDR8(y1) + x1;
    for (lcdint_t y = y1; y <= y2; y++)
    {
        *buf = color;
        buf += m_w;
    }
}

void NanoCanvas8::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, uint8_t color)
{
    x1 -= m_offsetx;
    y1 -= m_offsety;
    x2 -= m_offsetx;
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y1 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(x1,0);
    x2 = min(x2,(lcdint_t)m_w-1);
    uint8_t *buf = m_buf + YADDR8(y1) + x1;
    for (lcdint_t x = x1; x <= x2; x++)
    {
        *buf = color;
        buf++;
    }
}

void NanoCanvas8::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint8_t color)
{
    drawHLine(x1,y1,x2, color);
    drawHLine(x1,y2,x2, color);
    drawVLine(x1,y1,y2, color);
    drawVLine(x2,y1,y2, color);
}

void NanoCanvas8::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint8_t color)
{
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    x1 -= m_offsetx;
    y1 -= m_offsety;
    x2 -= m_offsetx;
    y2 -= m_offsety;
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(x1,0);
    x2 = min(x2,(lcdint_t)m_w-1);
    y1 = max(y1,0);
    y2 = min(y2,(lcdint_t)m_h-1);
    uint8_t *buf = m_buf + YADDR8(y1) + x1;
    for (lcdint_t y = y1; y <= y2; y++)
    {
        for (lcdint_t x = x1; x <= x2; x++)
        {
            *buf = color;
            buf++;
        }
        buf += ((lcdint_t)(m_w) - (x2 - x1 + 1));
    }
}

//#include <stdio.h>

void NanoCanvas8::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t offset = 0;
    /* calculate char rectangle */
    lcdint_t x1 = xpos - m_offsetx;
    lcdint_t y1 = ypos - m_offsety;
    lcdint_t x2 = x1 + (lcdint_t)w - 1;
    lcdint_t y2 = y1 + (lcdint_t)h - 1;
    /* clip char */
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;

    if (x1 < 0)
    {
        bitmap -= x1;
        x1 = 0;
    }
    if (y1 < 0)
    {
        bitmap += ((lcduint_t)(-y1) >> 3) * w;
        offset = ((-y1) & 0x07);
        y1 = 0;
    }
    if (y2 >= (lcdint_t)m_h)
    {
         y2 = (lcdint_t)m_h - 1;
    }
    if (x2 >= (lcdint_t)m_w)
    {
         x2 = (lcdint_t)m_w - 1;
    }
    uint8_t offset2 = 8 - offset;
    lcdint_t y = y1;
//    printf("[%d;%d] + [%d;%d], P1[%d;%d], P2[%d;%d]\n", xpos, ypos, m_offsetx, m_offsety, x1,y1,x2,y2);
//    printf("offset: 1=%d, 2=%d\n", offset, offset2);
    while ( y <= y2)
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint8_t data = pgm_read_byte( bitmap );
//            for (uint8_t n = offset; n < min(y2 - y + 1 + offset,8); n++)
            for (uint8_t n = 0; n < min(y2 - y + 1, 8); n++)
            {
//                if ( data & (1<<n) )
//                    m_buf[YADDR8(y + n - offset ) + x] = m_color;
//                else if (!(m_textMode & TEXT_MODE_TRANSPARENT))
//                    m_buf[YADDR8(y + n - offset ) + x] = 0x00;
                if ( data & (1<<(n + offset)) )
                    m_buf[YADDR8(y + n) + x] = m_color;
                else if (!(m_textMode & TEXT_MODE_TRANSPARENT))
                    m_buf[YADDR8(y + n) + x] = 0x00;
            }
            bitmap++;
        }
        bitmap += (w - (x2 - x1 + 1));
        y = y + offset2;
        offset = 0;
        offset2 = 8;
    }
}

void NanoCanvas8::printChar(uint8_t c)
{
    /* calculate char rectangle */
    
    lcdint_t x1 = m_cursorX;
    lcdint_t y1 = m_cursorY;
    lcdint_t x2 = x1 + (lcdint_t)s_fixedFont.width - 1;
    lcdint_t y2 = y1 + (lcdint_t)(s_fixedFont.pages << 3) - 1;
    /* clip char */
    if ((x2 < m_offsetx) || (x1 >= (lcdint_t)m_w + m_offsetx)) return;
    if ((y2 < m_offsety) || (y1 >= (lcdint_t)m_h + m_offsety)) return;

    c -= s_fixedFont.ascii_offset;
    if ( c > 224 )
    {
        c = 0;
    }
    uint16_t font_offset = c * s_fixedFont.pages * s_fixedFont.width;
    for (uint8_t page = 0; page < s_fixedFont.pages; page++ )
    {
        for ( uint8_t i = 0; i < s_fixedFont.width; i++ )
        {
            uint8_t data = pgm_read_byte( &s_fixedFont.data[font_offset] );
            for (uint8_t n = 0; n < 8; n++)
            {
                if ( data & (1<<n) )
                    putPixel(x1 + i, y1 + page * 8 + n, m_color);
                else if (!(m_textMode & TEXT_MODE_TRANSPARENT))
                    putPixel(x1 + i, y1 + page * 8 + n, 0x00);
            }
            font_offset++;
        }
    }
}

void NanoCanvas8::write(uint8_t c)
{
    if (c == '\n')
    {
        m_cursorY += (lcdint_t)(s_fixedFont.pages << 3);
        m_cursorX = 0;
    }
    else if (c == '\r')
    {
        // skip non-printed char
    }
    else
    {
        printChar( c );
        m_cursorX += (lcdint_t)s_fixedFont.width;
        if ((m_textMode & TEXT_MODE_WRAP) && (m_cursorX > ((lcdint_t)s_displayWidth - (lcdint_t)s_fixedFont.width)))
        {
            m_cursorY += (lcdint_t)(s_fixedFont.pages << 3);
            m_cursorX = 0;
        }
    }
}

void NanoCanvas8::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
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

void NanoCanvas8::blt()
{
    ssd1331_fastDrawBuffer8( m_offsetx, m_offsety, m_w, m_h, m_buf);
//    printf("==================================\n");
}

