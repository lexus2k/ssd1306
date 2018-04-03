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

#include "canvas.h"
#include "lcd/lcd_common.h"
#include "ssd1306.h"
#include "ssd1331_api.h"

#define swap_data(a, b ,type)  { type t = a; a = b; b = t; }

extern const uint8_t *s_font6x8;
extern SFixedFontInfo s_fixedFont;

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/* We need to use multiply operation, because there are displays on the market *
 * with resolution different from 2^N (160x128, 96x64, etc.)                   */
#define YADDR8(y) (static_cast<uint16_t>(y) * m_w)

void NanoCanvas8::putPixel(lcdint_t x, lcdint_t y)
{
    x -= offset.x;
    y -= offset.y;
    if ((x >= 0) && (y >= 0) && (x < (lcdint_t)m_w) && (y < (lcdint_t)m_h))
    {
        m_buf[YADDR8(y) + x] = m_color;
    }
}

void NanoCanvas8::putPixel(const NanoPoint &p)
{
    putPixel(p.x, p.y);
}

void NanoCanvas8::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if ((x1 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    y1 = max(y1,0);
    uint8_t *buf = m_buf + YADDR8(y1) + x1;
    y2 = min(y2,(lcdint_t)m_h-1) - y1;
    do
    {
        *buf = m_color;
        buf += m_w;
    }
    while (y2--);
}

void NanoCanvas8::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y1 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(x1,0);
    x2 = min(x2,(lcdint_t)m_w-1);
    uint8_t *buf = m_buf + YADDR8(y1) + x1;
    for (lcdint_t x = 0; x <= x2 - x1; x++)
    {
        *buf = m_color;
        buf++;
    }
}

void NanoCanvas8::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    drawHLine(x1,y1,x2);
    drawHLine(x1,y2,x2);
    drawVLine(x1,y1,y2);
    drawVLine(x2,y1,y2);
}

void NanoCanvas8::drawRect(const NanoRect &rect)
{
    drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

void NanoCanvas8::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    y2 -= offset.y;
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
            *buf = m_color;
            buf++;
        }
        buf += ((lcdint_t)(m_w) - (x2 - x1 + 1));
    }
}

void NanoCanvas8::fillRect(const NanoRect &rect)
{
    fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

//#include <stdio.h>

void NanoCanvas8::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t offs = 0;
    /* calculate char rectangle */
    lcdint_t x1 = xpos - offset.x;
    lcdint_t y1 = ypos - offset.y;
    lcdint_t x2 = x1 + (lcdint_t)w - 1;
    lcdint_t y2 = y1 + (lcdint_t)h - 1;
    /* clip bitmap */
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
        offs = ((-y1) & 0x07);
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
    uint8_t offs2 = 8 - offs;
    lcdint_t y = y1;
//    printf("[%d;%d] + [%d;%d], P1[%d;%d], P2[%d;%d]\n", xpos, ypos, offset.x, offset.y, x1,y1,x2,y2);
//    printf("offset: 1=%d, 2=%d\n", offs, offs2);
    while ( y <= y2)
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint8_t data = pgm_read_byte( bitmap );
            uint16_t addr = YADDR8(y) + x;
            for (uint8_t n = 0; n < min(y2 - y + 1, 8); n++)
            {
                if ( data & (1<<(n + offs)) )
                    m_buf[addr] = m_color;
                else if (!(m_textMode & CANVAS_MODE_TRANSPARENT))
                    m_buf[addr] = 0x00;
                addr += (lcduint_t)m_w;
            }
            bitmap++;
        }
        bitmap += (w - (x2 - x1 + 1));
        y = y + offs2;
        offs = 0;
        offs2 = 8;
    }
}

void NanoCanvas8::drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    /* calculate char rectangle */
    lcdint_t x1 = xpos - offset.x;
    lcdint_t y1 = ypos - offset.y;
    lcdint_t x2 = x1 + (lcdint_t)w - 1;
    lcdint_t y2 = y1 + (lcdint_t)h - 1;
    /* clip bitmap */
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;

    if (x1 < 0)
    {
        bitmap -= x1;
        x1 = 0;
    }
    if (y1 < 0)
    {
        bitmap += (lcduint_t)(-y1) * w;
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
    lcdint_t y = y1;
    while ( y <= y2 )
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( (data) || (!(m_textMode & CANVAS_MODE_TRANSPARENT)) )
            {
                m_buf[YADDR8(y) + x] = data;
            }
            bitmap++;
        }
        bitmap += (w - (x2 - x1 + 1));
        y++;
    }
}

void NanoCanvas8::printChar(uint8_t c)
{
    c -= s_fixedFont.ascii_offset;
    drawBitmap1(m_cursorX,
                m_cursorY,
                s_fixedFont.width,
                s_fixedFont.height,
                &s_fixedFont.data[ c * s_fixedFont.pages * s_fixedFont.width ] );
}

void NanoCanvas8::write(uint8_t c)
{
    if (c == '\n')
    {
        m_cursorY += (lcdint_t)s_fixedFont.height;
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
        if ((m_textMode & CANVAS_TEXT_WRAP) && (m_cursorX > ((lcdint_t)s_displayWidth - (lcdint_t)s_fixedFont.width)))
        {
            m_cursorY += (lcdint_t)s_fixedFont.height;
            m_cursorX = 0;
        }
    }
}

void NanoCanvas8::printFixed(lcdint_t xpos, lcdint_t y, const char *ch)
{
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
}

void NanoCanvas8::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch)
{
    m_cursorX = xpos;
    m_cursorY = y;
    for (;;)
    {
        char c = pgm_read_byte(ch);
        if (!c) break;
        write(c);
        ch++;
    }
}

void NanoCanvas8::clear()
{
    memset(m_buf, 0, YADDR8(m_h));
}

void NanoCanvas8::blt(lcdint_t x, lcdint_t y)
{
    ssd1331_drawBufferFast8( x, y, m_w, m_h, m_buf);
}

void NanoCanvas8::blt()
{
    ssd1331_drawBufferFast8( offset.x, offset.y, m_w, m_h, m_buf);
//    printf("==================================\n");
}

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

#define YADDR1(y) (static_cast<uint16_t>((y) >> 3) << m_p)
#define BANK_ADDR1(b) ((b) << m_p)

void NanoCanvas1::putPixel(lcdint_t x, lcdint_t y)
{
    x -= offset.x;
    y -= offset.y;
    if ((x<0) || (y<0)) return;
    if (( x >= (lcdint_t)m_w ) || ( y >= (lcdint_t)m_h)) return;
    if (m_color)
    {
        m_buf[YADDR1(y) + x] |= (1 << (y & 0x7));
    }
    else
    {
        m_buf[YADDR1(y) + x] &= ~(1 << (y & 0x7));
    }
}

void NanoCanvas1::putPixel(const NanoPoint &p)
{
    putPixel(p.x, p.y);
}

void NanoCanvas1::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    if (x2 < x1) swap_data(x2, x1, lcdint_t);
    x1 -= offset.x;
    x2 -= offset.x;
    y1 -= offset.y;
    if ((y1 >= (lcdint_t)m_h) || (y1 < 0)) return;
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    x1 = max(0, x1);
    x2 = min(x2, (lcdint_t)(m_w -1));
    uint16_t addr = YADDR1(y1) + x1;
    uint8_t mask = (1 << (y1 & 0x7));
    if (m_color)
    {
        do { m_buf[addr++] |= mask; } while (x2>x1++);
    }
    else
    {
        do { m_buf[addr++] &= ~mask; } while (x2>x1++);
    }
}

void NanoCanvas1::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    if (y2 < y1) swap_data(y2, y1, lcdint_t);
    x1 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if ((x1 >= (lcdint_t)m_w) || (x1 < 0)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    y1 = max(0, y1);
    y2 = min(y2, (lcdint_t)(m_h -1));

    uint16_t addr = YADDR1(y1) + x1;
    if ((y1 & 0xFFF8) == (y2 & 0xFFF8))
    {
        uint8_t mask = ((0xFF >> (0x07 + y1 - y2)) << (y1 & 0x07));
        if (m_color)
            m_buf[addr] |= mask;
        else
            m_buf[addr] &= ~mask;
        return;
    }
    if (m_color)
    {
        m_buf[addr] |= (0xFF << (y1 & 0x07));
        addr += m_w;
        while (addr<YADDR1(y2) + x1)
        {
            m_buf[addr] |= 0xFF;
            addr += m_w;
        }
        m_buf[addr] |= (0xFF >> (0x07 - (y2 & 0x07)));
    }
    else
    {
        m_buf[addr] &= ~(0xFF << (y1 & 0x07));
        addr += m_w;
        while (addr<YADDR1(y2) + x1)
        {
            m_buf[addr] &= 0;
            addr += m_w;
        }
        m_buf[addr] &= ~(0xFF >> (0x07 - (y2 & 0x07)));
    }
}

void NanoCanvas1::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    drawHLine(x1, y1, x2);
    drawHLine(x1, y2, x2);
    drawVLine(x1, y1, y2);
    drawVLine(x2, y1, y2);
}

void NanoCanvas1::drawRect(const NanoRect &rect)
{
    drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

void NanoCanvas1::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (x2 < x1) swap_data(x2, x1, lcdint_t);
    if (y2 < y1) swap_data(y2, y1, lcdint_t);
    x1 -= offset.x;
    x2 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(0, x1);
    x2 = min(x2, (lcdint_t)(m_w - 1));
    y1 = max(0, y1);
    y2 = min(y2, (lcdint_t)(m_h - 1));
    uint8_t bank1 = (y1 >> 3);
    uint8_t bank2 = (y2 >> 3);
    for (uint8_t bank = bank1; bank<=bank2; bank++)
    {
        uint8_t mask = 0xFF;
        if (bank1 == bank2)
        {
            mask = (mask >> ((y1 & 7) + 7 - (y2 & 7))) << (y1 & 7);
        }
        else if (bank1 == bank)
        {
            mask = (mask << (y1 & 7));
        }
        else if (bank2 == bank)
        {
            mask = (mask >> (7 - (y2 & 7)));
        }
        for (uint8_t x=x1; x<=x2; x++)
        {
            if (m_color)
            {
                m_buf[BANK_ADDR1(bank) + x] |= mask;
            }
            else
            {
                m_buf[BANK_ADDR1(bank) + x] &= ~mask;
            }
        }
    }
};

void NanoCanvas1::fillRect(const NanoRect &rect)
{
    fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

void NanoCanvas1::clear()
{
    memset(m_buf, 0, YADDR1(m_h));
}

// TODO: Not so fast implementation. needs to be optimized
void NanoCanvas1::drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    x -= offset.x;
    y -= offset.y;
    lcduint_t origin_width = w;
    uint8_t offs = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)m_h) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= (lcdint_t)m_w)  return;
    if (y < 0)
    {
         bitmap += ((lcduint_t)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         bitmap += -x;
         w += x;
         x = 0;
    } 
    uint8_t max_pages = (lcduint_t)(h + 15 - offs) >> 3;
    if ((lcduint_t)(y + (lcdint_t)h) > (lcduint_t)m_h)
    {                                                  
         h = (lcduint_t)(m_h - (lcduint_t)y);
    }
    if ((lcduint_t)(x + (lcdint_t)w) > (lcduint_t)m_w)
    {
         w = (lcduint_t)(m_w - (lcduint_t)x);
    }
    uint8_t pages = ((y + h - 1) >> 3) - (y >> 3) + 1;
    uint8_t j;
    lcduint_t i;

    for(j=0; j < pages; j++)
    {
        uint16_t addr = YADDR1(y + ((uint16_t)j<<3)) + x;
        if ( j == max_pages - 1 ) mainFlag = !offs;
        for( i=w; i > 0; i--)
        {       
            uint8_t data = 0;
            uint8_t mask = 0;
            if ( mainFlag )    { data |= (pgm_read_byte(bitmap) << offs); mask |= (0xFF << offs); }
            if ( complexFlag ) { data |= (pgm_read_byte(bitmap - origin_width) >> (8 - offs)); mask |= (0xFF >> (8 - offs)); }
            if (CANVAS_MODE_TRANSPARENT != (m_textMode & CANVAS_MODE_TRANSPARENT))
            {
                m_buf[addr] &= ~mask;
                m_buf[addr] |= m_color == BLACK ? ~data: data;
            }
            else
            {
                if (m_color == BLACK)
                    m_buf[addr] &= ~data;
                else
                    m_buf[addr] |= data;
            }
            bitmap++;
            addr++;
        }
        bitmap += origin_width - w;
        complexFlag = offs;
    }
}

void NanoCanvas1::printChar(uint8_t c)
{
    c -= s_fixedFont.ascii_offset;
    drawBitmap1(m_cursorX,
                m_cursorY,
                s_fixedFont.width,
                s_fixedFont.height,
                &s_fixedFont.data[ c * s_fixedFont.pages * s_fixedFont.width ] );
    /* calculate char rectangle */
}

void NanoCanvas1::write(uint8_t c)
{
    if (c == '\n')
    {
        m_cursorY += (lcdint_t)s_fixedFont.height;
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
        if ((m_textMode & CANVAS_TEXT_WRAP) && (m_cursorX > ((lcdint_t)s_displayWidth - (lcdint_t)s_fixedFont.width)))
        {
            m_cursorY += (lcdint_t)s_fixedFont.height;
            m_cursorX = 0;
        }
    }
}

void NanoCanvas1::printFixed(lcdint_t xpos, lcdint_t y, const char *ch)
{
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
}

void NanoCanvas1::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch)
{
    m_cursorX = xpos;
    m_cursorY = y;
    for (;;)
    {
        char c = pgm_read_byte(ch);
        if (!c) break;
        write(c);
        ch++;
    }
}

void NanoCanvas1::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawBufferFast( x, y, m_w, m_h, m_buf);
}

void NanoCanvas1::blt()
{
    ssd1306_drawBufferFast( offset.x, offset.y, m_w, m_h, m_buf);
}

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/* We need to use multiply operation, because there are displays on the market *
 * with resolution different from 2^N (160x128, 96x64, etc.)                   */
#define YADDR16(y) (static_cast<uint16_t>(y) * (m_w << 1))

void NanoCanvas16::putPixel(lcdint_t x, lcdint_t y)
{
    x -= offset.x;
    y -= offset.y;
    if ((x >= 0) && (y >= 0) && (x < (lcdint_t)m_w) && (y < (lcdint_t)m_h))
    {
        m_buf[YADDR16(y) + (x<<1)] = m_color >> 8;
        m_buf[YADDR16(y) + (x<<1) + 1] = m_color & 0xFF;
    }
}

void NanoCanvas16::putPixel(const NanoPoint &p)
{
    putPixel(p.x, p.y);
}

void NanoCanvas16::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if ((x1 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    y1 = max(y1,0);
    uint8_t *buf = m_buf + YADDR16(y1) + (x1 << 1);
    y2 = min(y2,(lcdint_t)m_h-1) - y1;
    do
    {
        buf[0] = m_color >> 8;
        buf[1] = m_color & 0xFF;
        buf += (m_w<<1);
    }
    while (y2--);
}

void NanoCanvas16::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y1 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(x1,0);
    x2 = min(x2,(lcdint_t)m_w-1);
    uint8_t *buf = m_buf + YADDR16(y1) + (x1<<1);
    for (lcdint_t x = 0; x <= x2 - x1; x++)
    {
        buf[0] = m_color >> 8;
        buf[1] = m_color & 0xFF;
        buf+=2;
    }
}

void NanoCanvas16::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    drawHLine(x1,y1,x2);
    drawHLine(x1,y2,x2);
    drawVLine(x1,y1,y2);
    drawVLine(x2,y1,y2);
}

void NanoCanvas16::drawRect(const NanoRect &rect)
{
    drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

void NanoCanvas16::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    y2 -= offset.y;
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(x1,0);
    x2 = min(x2,(lcdint_t)m_w-1);
    y1 = max(y1,0);
    y2 = min(y2,(lcdint_t)m_h-1);
    uint8_t *buf = m_buf + YADDR16(y1) + (x1<<1);
    for (lcdint_t y = y1; y <= y2; y++)
    {
        for (lcdint_t x = x1; x <= x2; x++)
        {
            buf[0] = m_color >> 8;
            buf[1] = m_color & 0xFF;
            buf+=2;
        }
        buf += ( ((lcdint_t)(m_w) - (x2 - x1 + 1)) <<1 );
    }
}

void NanoCanvas16::fillRect(const NanoRect &rect)
{
    fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

//#include <stdio.h>

void NanoCanvas16::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t offs = 0;
    /* calculate char rectangle */
    lcdint_t x1 = xpos - offset.x;
    lcdint_t y1 = ypos - offset.y;
    lcdint_t x2 = x1 + (lcdint_t)w - 1;
    lcdint_t y2 = y1 + (lcdint_t)h - 1;
    /* clip bitmap */
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
        offs = ((-y1) & 0x07);
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
    uint8_t offs2 = 8 - offs;
    lcdint_t y = y1;
//    printf("[%d;%d] + [%d;%d], P1[%d;%d], P2[%d;%d]\n", xpos, ypos, offset.x, offset.y, x1,y1,x2,y2);
//    printf("offset: 1=%d, 2=%d\n", offs, offs2);
    while ( y <= y2)
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint8_t data = pgm_read_byte( bitmap );
            uint16_t addr = YADDR16(y) + (x<<1);
            for (uint8_t n = 0; n < min(y2 - y + 1, 8); n++)
            {
                if ( data & (1<<(n + offs)) )
                {
                    m_buf[addr] = m_color >> 8;
                    m_buf[addr+1] = m_color & 0xFF;
                }
                else if (!(m_textMode & CANVAS_MODE_TRANSPARENT))
                {
                    m_buf[addr] = 0x00;
                    m_buf[addr+1] = 0x00;
                }
                addr += ((lcduint_t)m_w << 1);
            }
            bitmap++;
        }
        bitmap += (w - (x2 - x1 + 1));
        y = y + offs2;
        offs = 0;
        offs2 = 8;
    }
}

void NanoCanvas16::drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    /* calculate char rectangle */
    lcdint_t x1 = xpos - offset.x;
    lcdint_t y1 = ypos - offset.y;
    lcdint_t x2 = x1 + (lcdint_t)w - 1;
    lcdint_t y2 = y1 + (lcdint_t)h - 1;
    /* clip bitmap */
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;

    if (x1 < 0)
    {
        bitmap -= x1;
        x1 = 0;
    }
    if (y1 < 0)
    {
        bitmap += (lcduint_t)(-y1) * w;
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
    lcdint_t y = y1;
    while ( y <= y2 )
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( (data) || (!(m_textMode & CANVAS_MODE_TRANSPARENT)) )
            {
                uint16_t color = (((uint16_t)data & 0b11100000) << 8) |
                                 (((uint16_t)data & 0b00011100) << 6) |
                                 (((uint16_t)data & 0b00000011) << 3);
                m_buf[YADDR16(y) + (x<<1)] = color;
            }
            bitmap++;
        }
        bitmap += (w - (x2 - x1 + 1));
        y++;
    }
}

void NanoCanvas16::printChar(uint8_t c)
{
    c -= s_fixedFont.ascii_offset;
    drawBitmap1(m_cursorX,
                m_cursorY,
                s_fixedFont.width,
                s_fixedFont.height,
                &s_fixedFont.data[ c * s_fixedFont.pages * s_fixedFont.width ] );
}

void NanoCanvas16::write(uint8_t c)
{
    if (c == '\n')
    {
        m_cursorY += (lcdint_t)s_fixedFont.height;
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
        if ((m_textMode & CANVAS_TEXT_WRAP) && (m_cursorX > ((lcdint_t)s_displayWidth - (lcdint_t)s_fixedFont.width)))
        {
            m_cursorY += (lcdint_t)s_fixedFont.height;
            m_cursorX = 0;
        }
    }
}

void NanoCanvas16::printFixed(lcdint_t xpos, lcdint_t y, const char *ch)
{
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
}

void NanoCanvas16::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch)
{
    m_cursorX = xpos;
    m_cursorY = y;
    for (;;)
    {
        char c = pgm_read_byte(ch);
        if (!c) break;
        write(c);
        ch++;
    }
}

void NanoCanvas16::clear()
{
    memset(m_buf, 0, YADDR16(m_h));
}

void NanoCanvas16::blt(lcdint_t x, lcdint_t y)
{
    ssd1331_drawBufferFast16( x, y, m_w, m_h, m_buf);
}

void NanoCanvas16::blt()
{
    ssd1331_drawBufferFast16( offset.x, offset.y, m_w, m_h, m_buf);
//    printf("==================================\n");
}
