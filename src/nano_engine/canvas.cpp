/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

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

extern const uint8_t *s_font6x8;
extern "C" SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern "C" uint8_t g_ssd1306_unicode;
#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <uint8_t BPP>
void NanoCanvasOps<BPP>::putPixel(const NanoPoint &p)
{
    putPixel(p.x, p.y);
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    drawHLine(x1, y1, x2);
    drawHLine(x1, y2, x2);
    drawVLine(x1, y1, y2);
    drawVLine(x2, y1, y2);
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::drawRect(const NanoRect &rect)
{
    drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    lcduint_t  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    lcduint_t  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    lcduint_t  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            ssd1306_swap_data(x1, x2, lcdint_t);
            ssd1306_swap_data(y1, y2, lcdint_t);
        }
        for(; y1<=y2; y1++)
        {
            err += dx;
            if (err >= dy)
            {
                 err -= dy;
                 x1 < x2 ? x1++: x1--;
            }
            putPixel( x1, y1 );
        }
    }
    else
    {
        if (x1 > x2)
        {
            ssd1306_swap_data(x1, x2, lcdint_t);
            ssd1306_swap_data(y1, y2, lcdint_t);
        }
        for(; x1<=x2; x1++)
        {
            err += dy;
            if (err >= dx)
            {
                 err -= dx;
                 if (y1 < y2) y1++; else y1--;
            }
            putPixel( x1, y1 );
        }
    }
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::drawLine(const NanoRect &rect)
{
    drawLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::fillRect(const NanoRect &rect)
{
    fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
uint8_t NanoCanvasOps<BPP>::printChar(uint8_t c)
{
    uint16_t unicode = ssd1306_unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    uint8_t mode = m_textMode;
    for (uint8_t i = 0; i<(m_fontStyle == STYLE_BOLD ? 2: 1); i++)
    {
        drawBitmap1(m_cursorX + i,
                    m_cursorY,
                    char_info.width,
                    char_info.height,
                    char_info.glyph );
        m_textMode |= CANVAS_MODE_TRANSPARENT;
    }
    m_textMode = mode;
    m_cursorX += (lcdint_t)(char_info.width + char_info.spacing);
    if ( ( (m_textMode & CANVAS_TEXT_WRAP_LOCAL) && (m_cursorX > ((lcdint_t)m_w - (lcdint_t)s_fixedFont.h.width) ) )
       || ( (m_textMode & CANVAS_TEXT_WRAP) && (m_cursorX > ((lcdint_t)ssd1306_lcd.width - (lcdint_t)s_fixedFont.h.width)) ) )
    {
        m_cursorY += (lcdint_t)s_fixedFont.h.height;
        m_cursorX = 0;
        if ( (m_textMode & CANVAS_TEXT_WRAP_LOCAL) && (m_cursorY > ((lcdint_t)m_h - (lcdint_t)s_fixedFont.h.height)) )
        {
            m_cursorY = 0;
        }
    }
    return 1;
}

template <uint8_t BPP>
size_t NanoCanvasOps<BPP>::write(uint8_t c)
{
    if (c == '\n')
    {
        m_cursorY += (lcdint_t)s_fixedFont.h.height;
        m_cursorX = 0;
    }
    else if (c == '\r')
    {
        // skip non-printed char
    }
    else
    {
        return printChar( c );
    }
    return 1;
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    m_fontStyle = style;
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
}

template <uint8_t BPP>
void NanoCanvasOps<BPP>::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    m_fontStyle = style;
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

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

#ifdef CONFIG_MULTIPLICATION_NOT_SUPPORTED
#define YADDR1(y) (static_cast<uint16_t>((y) >> 3) << m_p)
#define BANK_ADDR1(b) ((b) << m_p)
#else
#define YADDR1(y) (static_cast<uint16_t>((y) >> 3) * m_w)
#define BANK_ADDR1(b) ((b) * m_w)
#endif

template <>
void NanoCanvasOps<1>::putPixel(lcdint_t x, lcdint_t y)
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

template <>
void NanoCanvasOps<1>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    if (x2 < x1) ssd1306_swap_data(x2, x1, lcdint_t);
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

template <>
void NanoCanvasOps<1>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    if (y2 < y1) ssd1306_swap_data(y2, y1, lcdint_t);
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

template <>
void NanoCanvasOps<1>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (x2 < x1) ssd1306_swap_data(x2, x1, lcdint_t);
    if (y2 < y1) ssd1306_swap_data(y2, y1, lcdint_t);
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

template <>
void NanoCanvasOps<1>::clear()
{
    memset(m_buf, 0, YADDR1(m_h));
}

// TODO: Not so fast implementation. needs to be optimized
template <>
void NanoCanvasOps<1>::drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    x -= offset.x;
    y -= offset.y;
    lcduint_t origin_width = w;
    uint8_t offs = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)m_h) return;
    if ((int16_t)x + (int16_t)w <= 0) return;
    if ((int16_t)x >= (int16_t)m_w)  return;
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

template <>
void NanoCanvasOps<1>::drawXBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    x -= offset.x;
    y -= offset.y;
    lcduint_t origin_width = w;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)m_h) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= (lcdint_t)m_w)  return;

    delay(100);

    uint8_t start_bit = 0;
    lcduint_t pitch_delta = 0;
    if (y < 0)
    {
        bitmap += /*pitch*/((origin_width + 7) >> 3) * (lcduint_t)(-y);
        h += y;
        y = 0;
    }
    if (x < 0)
    {
        bitmap += ((lcduint_t)(-x)) / 8;
        start_bit = ((lcduint_t)(-x)) & 0x07;
        w += x;
        x = 0;
    }
    if ((lcduint_t)(y + (lcdint_t)h) > (lcduint_t)m_h)
    {
        h = (lcduint_t)(m_h - (lcduint_t)y);
    }
    if ((lcduint_t)(x + (lcdint_t)w) > (lcduint_t)m_w)
    {
        w = (lcduint_t)(m_w - (lcduint_t)x);
    }
    pitch_delta = ((origin_width + 7 - start_bit) >> 3) - ((w + 7) >> 3);

    for(lcduint_t j = 0; j < h; j++)
    {
        uint8_t bit = start_bit;
        for(lcduint_t i = 0; i < w; i++)
        {
            uint8_t data = 0;
            data = (pgm_read_byte(bitmap) >> bit) & 0x01;
            if (data)
            {
                if (m_color == BLACK)
                    m_buf[YADDR1(y + j) + (x + i)] &= ~(1 << ((y + j) & 0x7));
                else
                    m_buf[YADDR1(y + j) + (x + i)] |= (1 << ((y + j) & 0x7));
            }
            else if (CANVAS_MODE_TRANSPARENT != (m_textMode & CANVAS_MODE_TRANSPARENT))
            {
                if (m_color == BLACK)
                    m_buf[YADDR1(y + j) + (x + i)] |= (1 << ((y + j) & 0x7));
                else
                    m_buf[YADDR1(y + j) + (x + i)] &= ~(1 << ((y + j) & 0x7));
            }
            bit++;
            if (bit >= 8)
            {
                bitmap++;
                bit=0;
            }
        }
        if (bit)
        {
            bitmap++;
        }
        bitmap += pitch_delta;
    }
}

template <>
void NanoCanvasOps<1>::begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
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
}

//                 NANO CANVAS 1

void NanoCanvas1::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawBufferFast(x, y, m_w, m_h, m_buf);
}

void NanoCanvas1::blt()
{
    ssd1306_drawBufferFast(offset.x, offset.y, m_w, m_h, m_buf);
}

void NanoCanvas1::blt(const NanoRect &rect)
{
    // TODO: NOT IMPLEMENTED
}

//                 NANO CANVAS 1_8

void NanoCanvas1_8::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawMonoBuffer8(x, y, m_w, m_h, m_buf);
}

void NanoCanvas1_8::blt()
{
    ssd1306_drawMonoBuffer8(offset.x, offset.y, m_w, m_h, m_buf);
}

void NanoCanvas1_8::blt(const NanoRect &rect)
{
    // TODO: NOT IMPLEMENTED
}

//                 NANO CANVAS 1_16

void NanoCanvas1_16::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawMonoBuffer16(x, y, m_w, m_h, m_buf);
}

void NanoCanvas1_16::blt()
{
    ssd1306_drawMonoBuffer16(offset.x, offset.y, m_w, m_h, m_buf);
}

void NanoCanvas1_16::blt(const NanoRect &rect)
{
    // TODO: NOT IMPLEMENTED
}

/////////////////////////////////////////////////////////////////////////////////
//
//                           4-BIT GRAY GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/* We need to use multiply operation, because there are displays on the market *
 * with resolution different from 2^N (160x128, 96x64, etc.)                   */
#define YADDR4(y) (static_cast<uint32_t>(y) * m_w / 2)
#define BITS_SHIFT4(x) ((x & 1) ? 4 : 0)

template <>
void NanoCanvasOps<4>::putPixel(lcdint_t x, lcdint_t y)
{
    x -= offset.x;
    y -= offset.y;
    if ((x >= 0) && (y >= 0) && (x < (lcdint_t)m_w) && (y < (lcdint_t)m_h))
    {
        m_buf[YADDR4(y) + x / 2] &= ~(0x0F << BITS_SHIFT4(x));
        m_buf[YADDR4(y) + x / 2] |= (m_color & 0x0F) << BITS_SHIFT4(x);
    }
}

template <>
void NanoCanvasOps<4>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if ((x1 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;
    y1 = max(y1,0);
    y2 = min(y2,(lcdint_t)m_h-1) - y1;
    uint8_t *buf = m_buf + YADDR4(y1) + x1 / 2;
    do
    {
        *buf &= ~(0x0F << BITS_SHIFT4(x1));
        *buf |= ((m_color & 0x0F) << BITS_SHIFT4(x1));
        buf += m_w / 2;
    }
    while (y2--);
}

template <>
void NanoCanvasOps<4>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
    }
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y1 < 0) || (y1 >= (lcdint_t)m_h)) return;
    x1 = max(x1,0);
    x2 = min(x2,(lcdint_t)m_w-1);
    uint8_t *buf = m_buf + YADDR4(y1) + x1 / 2;
    for (lcdint_t x = x1; x <= x2; x++)
    {
        *buf &= ~(0x0F << BITS_SHIFT4(x));
        *buf |= ((m_color & 0x0F) << BITS_SHIFT4(x));
        if ( x & 1 )
        {
            buf++;
        }
    }
}

template <>
void NanoCanvasOps<4>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
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
    uint8_t *buf = m_buf + YADDR4(y1) + x1 / 2;
    for (lcdint_t y = y1; y <= y2; y++)
    {
        for (lcdint_t x = x1; x <= x2; x++)
        {
            *buf &= ~(0x0F << BITS_SHIFT4(x));
            *buf |= ((m_color & 0xF) << BITS_SHIFT4(x));
            if ( x & 1 )
            {
                buf++;
            }
        }
        buf += ((lcdint_t)(m_w) - (x2 - x1 + ((x2 - x1) & 0x1))) / 2;
    }
}

template <>
void NanoCanvasOps<4>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    lcdint_t xb1 = 0;
    lcdint_t yb1 = 0;
    lcdint_t xb2 = (lcdint_t)w - 1;
    lcdint_t yb2 = (lcdint_t)h - 1;
    /* calculate char rectangle */
    lcdint_t x1 = xpos - offset.x;
    lcdint_t y1 = ypos - offset.y;
    lcdint_t x2 = x1 + xb2;
    lcdint_t y2 = y1 + yb2;
    /* clip bitmap */
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;

    if (x1 < 0)
    {
        xb1 -= x1;
        x1 = 0;
    }
    if (y1 < 0)
    {
        yb1 -= y1;
        y1 = 0;
    }
    if (y2 >= (lcdint_t)m_h)
    {
         yb2 -= (y2 - (lcdint_t)m_h + 1);
         y2 = (lcdint_t)m_h - 1;
    }
    if (x2 >= (lcdint_t)m_w)
    {
         xb2 -= (x2 - (lcdint_t)m_w + 1);
         x2 = (lcdint_t)m_w - 1;
    }
    for ( lcdint_t y = y1; y <= y2; y++ )
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint16_t src_addr1 = xb1 + x - x1 + ((yb1 + y - y1) / 8) * w;
            uint8_t src_bit1 = (yb1 + y - y1) & 0x07;
            uint8_t data = pgm_read_byte( &bitmap[ src_addr1 ] );
            uint16_t addr = YADDR4(y) + x / 2;
            if (data & (1 << src_bit1))
            {
                data = m_color & 0x0F;
                m_buf[ addr ] &= ~(0x0F << BITS_SHIFT4(x));
                m_buf[ addr ] |= data << BITS_SHIFT4(x);
            }
            else if (!(m_textMode & CANVAS_MODE_TRANSPARENT))
            {
                data = 0x00;
                m_buf[ addr ] &= ~(0x0F << BITS_SHIFT4(x));
            }
        }
    }
}

template <>
void NanoCanvasOps<4>::drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    lcdint_t xb1 = 0;
    lcdint_t yb1 = 0;
    lcdint_t xb2 = (lcdint_t)w - 1;
    lcdint_t yb2 = (lcdint_t)h - 1;
    /* calculate char rectangle */
    lcdint_t x1 = xpos - offset.x;
    lcdint_t y1 = ypos - offset.y;
    lcdint_t x2 = x1 + xb2;
    lcdint_t y2 = y1 + yb2;
    /* clip bitmap */
    if ((x2 < 0) || (x1 >= (lcdint_t)m_w)) return;
    if ((y2 < 0) || (y1 >= (lcdint_t)m_h)) return;

    if (x1 < 0)
    {
        xb1 -= x1;
        x1 = 0;
    }
    if (y1 < 0)
    {
        yb1 -= y1;
        y1 = 0;
    }
    if (y2 >= (lcdint_t)m_h)
    {
         yb2 -= (y2 - (lcdint_t)m_h + 1);
         y2 = (lcdint_t)m_h - 1;
    }
    if (x2 >= (lcdint_t)m_w)
    {
         xb2 -= (x2 - (lcdint_t)m_w + 1);
         x2 = (lcdint_t)m_w - 1;
    }
    for ( lcdint_t y = y1; y <= y2; y++ )
    {
        for ( lcdint_t x = x1; x <= x2; x++ )
        {
            uint16_t src_addr8 = xb1 + x - x1 + ((yb1 + y - y1)) * w;
            uint8_t data = pgm_read_byte( &bitmap[ src_addr8 ] );
            uint16_t addr = YADDR4(y) + x / 2;
            if ( (data) || (!(m_textMode & CANVAS_MODE_TRANSPARENT)) )
            {
                data = RGB8_TO_GRAY4(data);
                m_buf[ addr ] &= ~(0x0F << BITS_SHIFT4(x));
                m_buf[ addr ] |= (data & 0x0F) << BITS_SHIFT4(x);
            }
        }
    }
}

template <>
void NanoCanvasOps<4>::clear()
{
    memset(m_buf, 0, YADDR4(m_h));
}

/* This method must be implemented always after clear() */
template <>
void NanoCanvasOps<4>::begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
{
    m_w = w;
    m_h = h;
    offset.x = 0;
    offset.y = 0;
    m_cursorX = 0;
    m_cursorY = 0;
    m_color = 0xFF; // white color by default
    m_textMode = 0;
    m_buf = bytes;
    clear();
}

//                 NANO CANVAS 1_8

void NanoCanvas1_4::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawBuffer1_4(x, y, m_w, m_h, m_buf);
}

void NanoCanvas1_4::blt()
{
    ssd1306_drawBuffer1_4(offset.x, offset.y, m_w, m_h, m_buf);
}

void NanoCanvas1_4::blt(const NanoRect &rect)
{
    // TODO: NOT IMPLEMENTED
}

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/* We need to use multiply operation, because there are displays on the market *
 * with resolution different from 2^N (160x128, 96x64, etc.)                   */
#define YADDR8(y) (static_cast<uint32_t>(y) * m_w)

template <>
void NanoCanvasOps<8>::putPixel(lcdint_t x, lcdint_t y)
{
    x -= offset.x;
    y -= offset.y;
    if ((x >= 0) && (y >= 0) && (x < (lcdint_t)m_w) && (y < (lcdint_t)m_h))
    {
        m_buf[YADDR8(y) + x] = m_color;
    }
}

template <>
void NanoCanvasOps<8>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
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

template <>
void NanoCanvasOps<8>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
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

template <>
void NanoCanvasOps<8>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
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

template <>
void NanoCanvasOps<8>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
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

template <>
void NanoCanvasOps<8>::drawXBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    x -= offset.x;
    y -= offset.y;
    lcduint_t origin_width = w;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)m_h) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= (lcdint_t)m_w)  return;

    uint8_t start_bit = 0;
    lcduint_t pitch_delta = 0;
    if (y < 0)
    {
        bitmap += /*pitch*/((origin_width + 7) >> 3) * (lcduint_t)(-y);
        h += y;
        y = 0;
    }
    if (x < 0)
    {
        bitmap += ((lcduint_t)(-x)) / 8;
        start_bit = ((lcduint_t)(-x)) & 0x07;
        w += x;
        x = 0;
    }
    if ((lcduint_t)(y + (lcdint_t)h) > (lcduint_t)m_h)
    {
        h = (lcduint_t)(m_h - (lcduint_t)y);
    }
    if ((lcduint_t)(x + (lcdint_t)w) > (lcduint_t)m_w)
    {
        w = (lcduint_t)(m_w - (lcduint_t)x);
    }
    pitch_delta = ((origin_width + 7 - start_bit) >> 3) - ((w + 7) >> 3);

    for(lcduint_t j = 0; j < h; j++)
    {
        uint8_t bit = start_bit;
        for(lcduint_t i = 0; i < w; i++)
        {
            uint8_t data = 0;
            data = (pgm_read_byte(bitmap) >> bit) & 0x01;
            if (data)
            {
                m_buf[YADDR8(y + j) + (x + i)] = m_color;
            }
            else if (CANVAS_MODE_TRANSPARENT != (m_textMode & CANVAS_MODE_TRANSPARENT))
            {
                m_buf[YADDR8(y + j) + (x + i)] = 0x00;
            }
            bit++;
            if (bit >= 8)
            {
                bitmap++;
                bit=0;
            }
        }
        if (bit)
        {
            bitmap++;
        }
        bitmap += pitch_delta;
    }
}

template <>
void NanoCanvasOps<8>::drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
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

template <>
void NanoCanvasOps<8u>::clear()
{
    memset(m_buf, 0, YADDR8(m_h));
}

/* This method must be implemented always after clear() */
template <>
void NanoCanvasOps<8>::begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
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
}

//                NANO CANVAS 8

void NanoCanvas8::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawBufferFast8(x, y, m_w, m_h, m_buf);
}

void NanoCanvas8::blt()
{
    ssd1306_drawBufferFast8(offset.x, offset.y, m_w, m_h, m_buf);
}

void NanoCanvas8::blt(const NanoRect &rect)
{
    ssd1306_drawBufferEx8(offset.x + rect.p1.x,
                          offset.y + rect.p1.y,
                          rect.width(),
                          rect.height(),
                          m_w,
                          m_buf + rect.p1.x + rect.p1.y * m_w );
}

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/* We need to use multiply operation, because there are displays on the market *
 * with resolution different from 2^N (160x128, 96x64, etc.)                   */
#define YADDR16(y) (static_cast<uint32_t>(y) * (m_w << 1))

template <>
void NanoCanvasOps<16>::putPixel(lcdint_t x, lcdint_t y)
{
    x -= offset.x;
    y -= offset.y;
    if ((x >= 0) && (y >= 0) && (x < (lcdint_t)m_w) && (y < (lcdint_t)m_h))
    {
        m_buf[YADDR16(y) + (x<<1)] = m_color >> 8;
        m_buf[YADDR16(y) + (x<<1) + 1] = m_color & 0xFF;
    }
}

template <>
void NanoCanvasOps<16>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    y2 -= offset.y;
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
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

template <>
void NanoCanvasOps<16>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    x1 -= offset.x;
    y1 -= offset.y;
    x2 -= offset.x;
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
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

template <>
void NanoCanvasOps<16>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
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

template <>
void NanoCanvasOps<16>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
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

template <>
void NanoCanvasOps<16>::drawXBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    x -= offset.x;
    y -= offset.y;
    lcduint_t origin_width = w;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)m_h) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= (lcdint_t)m_w)  return;

    uint8_t start_bit = 0;
    lcduint_t pitch_delta = 0;
    if (y < 0)
    {
        bitmap += /*pitch*/((origin_width + 7) >> 3) * (lcduint_t)(-y);
        h += y;
        y = 0;
    }
    if (x < 0)
    {
        bitmap += ((lcduint_t)(-x)) / 8;
        start_bit = ((lcduint_t)(-x)) & 0x07;
        w += x;
        x = 0;
    }
    if ((lcduint_t)(y + (lcdint_t)h) > (lcduint_t)m_h)
    {
        h = (lcduint_t)(m_h - (lcduint_t)y);
    }
    if ((lcduint_t)(x + (lcdint_t)w) > (lcduint_t)m_w)
    {
        w = (lcduint_t)(m_w - (lcduint_t)x);
    }
    pitch_delta = ((origin_width + 7 - start_bit) >> 3) - ((w + 7) >> 3);

    for(lcduint_t j = 0; j < h; j++)
    {
        uint8_t bit = start_bit;
        for(lcduint_t i = 0; i < w; i++)
        {
            uint8_t data = 0;
            data = (pgm_read_byte(bitmap) >> bit) & 0x01;
            if (data)
            {
                m_buf[YADDR16(y + j) + ((x + i)<<1)] = m_color >> 8;
                m_buf[YADDR16(y + j) + ((x + i)<<1) + 1] = m_color & 0xFF;
            }
            else if (CANVAS_MODE_TRANSPARENT != (m_textMode & CANVAS_MODE_TRANSPARENT))
            {
                m_buf[YADDR16(y + j) + ((x + i)<<1)] = 0x00;
                m_buf[YADDR16(y + j) + ((x + i)<<1) + 1] = 0x00;
            }
            bit++;
            if (bit >= 8)
            {
                bitmap++;
                bit=0;
            }
        }
        if (bit)
        {
            bitmap++;
        }
        bitmap += pitch_delta;
    }
}

template <>
void NanoCanvasOps<16>::drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
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

template <>
void NanoCanvasOps<16>::clear()
{
    memset(m_buf, 0, YADDR16(m_h));
}

template <>
void NanoCanvasOps<16>::begin(lcdint_t w, lcdint_t h, uint8_t *bytes)
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
}

/////////////////////////////////////////////////////////////////////////////////
//
//                      NanoCanvasOps class initiation
//
/////////////////////////////////////////////////////////////////////////////////

template class NanoCanvasOps<1>;
template class NanoCanvasOps<4>;
template class NanoCanvasOps<8>;
template class NanoCanvasOps<16>;


//              NANO CANVAS 16

void NanoCanvas16::blt(lcdint_t x, lcdint_t y)
{
    ssd1306_drawBufferFast16(x, y, m_w, m_h, m_buf);
}

void NanoCanvas16::blt()
{
    ssd1306_drawBufferFast16(offset.x, offset.y, m_w, m_h, m_buf);
}

void NanoCanvas16::blt(const NanoRect &rect)
{
    ssd1306_drawBufferEx16(offset.x + rect.p1.x,
                           offset.y + rect.p1.y,
                           rect.width(),
                           rect.height(),
                           m_w<<1,
                           m_buf + (rect.p1.x<<1) + rect.p1.y * (m_w<<1) );
}
