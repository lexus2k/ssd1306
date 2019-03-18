/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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

///////////////////////////////////////////////////////////////////////////////
////// GENERIC FUNCTIONS APPLICABLE FOR ALL DISPLAY TYPES /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "lcd/lcd_common.h"
#include "ssd1306_hal/io.h"
#include "ssd1306_generic.h"

// TODO: remove
#include "lcd/ssd1306_commands.h"

uint8_t s_ssd1306_invertByte = 0x00000000;
//const uint8_t *s_font6x8 = &ssd1306xled_font6x8[4];
//extern lcduint_t ssd1306_cursorX;
//extern lcduint_t ssd1306_cursorY;
extern SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern uint8_t g_ssd1306_unicode;
#endif

#if 0
void ssd1306_fillScreen(uint8_t fill_Data)
{
    fill_Data ^= s_ssd1306_invertByte;
    ssd1306_lcd.set_block(0, 0, 0);
    for(uint8_t m=(ssd1306_lcd.height >> 3); m>0; m--)
    {
        for(uint8_t n=ssd1306_lcd.width; n>0; n--)
        {
            ssd1306_lcd.send_pixels1(fill_Data);
        }
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

void ssd1306_clearScreen()
{
    ssd1306_lcd.set_block(0, 0, 0);
    for(uint8_t m=(ssd1306_lcd.height >> 3); m>0; m--)
    {
        for(uint8_t n=ssd1306_lcd.width; n>0; n--)
        {
            ssd1306_lcd.send_pixels1( s_ssd1306_invertByte );
        }
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

uint8_t ssd1306_printFixed(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t ldata;
        if ( (x > ssd1306_lcd.width - s_fixedFont.h.width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == s_fixedFont.pages)
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        uint16_t unicode;
        do
        {
            unicode = ssd1306_unicode16FromUtf8(ch[j]);
            j++;
        } while ( unicode == SSD1306_MORE_CHARS_REQUIRED );
        SCharInfo char_info;
        ssd1306_getCharBitmap(unicode, &char_info);
        ldata = 0;
        x += char_info.width + char_info.spacing;
        if (char_info.height > page_offset * 8)
        {
            char_info.glyph += page_offset * char_info.width;
            for( i = char_info.width; i>0; i--)
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = pgm_read_byte(&char_info.glyph[0]);
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = pgm_read_byte(&char_info.glyph[0]);
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = pgm_read_byte(&char_info.glyph[1]);
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                ssd1306_lcd.send_pixels1(data^s_ssd1306_invertByte);
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for (i = 0; i < char_info.spacing; i++)
            ssd1306_lcd.send_pixels1(s_ssd1306_invertByte);
    }
    ssd1306_intf.stop();
    return j;
}

uint8_t ssd1306_printFixed_oldStyle(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if( (x > ssd1306_lcd.width - s_fixedFont.h.width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == s_fixedFont.pages)
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        c = ch[j];
        if ( c >= s_fixedFont.h.ascii_offset )
        {
            c -= s_fixedFont.h.ascii_offset;
        }
        ldata = 0;
        offset = (c * s_fixedFont.pages + page_offset) * s_fixedFont.h.width;
        for( i=s_fixedFont.h.width; i>0; i--)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.primary_table[offset]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.primary_table[offset + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            ssd1306_lcd.send_pixels1(data^s_ssd1306_invertByte);
            offset++;
        }
        x += s_fixedFont.h.width;
        j++;
    }
    ssd1306_intf.stop();
    return j;
}

uint8_t ssd1306_printFixed2x(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if( (x > ssd1306_lcd.width - (s_fixedFont.h.width << 1)) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == (s_fixedFont.pages << 1))
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        c = ch[j];
        if ( c >= 32 )
        {
            c -= 32;
        }
        ldata = 0;
        offset = (c * s_fixedFont.pages + (page_offset >> 1)) * s_fixedFont.h.width;
        for( i=s_fixedFont.h.width; i>0; i--)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.primary_table[offset]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.primary_table[offset + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            if (page_offset & 1) data >>= 4;
            data = ((data & 0x01) ? 0x03: 0x00) |
                   ((data & 0x02) ? 0x0C: 0x00) |
                   ((data & 0x04) ? 0x30: 0x00) |
                   ((data & 0x08) ? 0xC0: 0x00);
            ssd1306_lcd.send_pixels1(data^s_ssd1306_invertByte);
            ssd1306_lcd.send_pixels1(data^s_ssd1306_invertByte);
            offset++;
        }
        x += (s_fixedFont.h.width << 1);
        j++;
    }
    ssd1306_intf.stop();
    return j;
}


uint8_t ssd1306_printFixedN(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style, uint8_t factor)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t ldata;
        if( (x > ssd1306_lcd.width - (s_fixedFont.h.width << factor)) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == (s_fixedFont.pages << factor))
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        uint16_t unicode;
        do
        {
            unicode = ssd1306_unicode16FromUtf8(ch[j]);
            j++;
        } while ( unicode == SSD1306_MORE_CHARS_REQUIRED );
        SCharInfo char_info;
        ssd1306_getCharBitmap(unicode, &char_info);
        ldata = 0;
        x += ((char_info.width + char_info.spacing) << factor);
        if (char_info.height > (page_offset >> factor) * 8)
        {
            char_info.glyph += (page_offset >> factor) * char_info.width;
            for( i=char_info.width; i>0; i--)
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = pgm_read_byte(char_info.glyph);
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = pgm_read_byte(char_info.glyph);
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = pgm_read_byte(char_info.glyph+1);
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                if ( factor > 0 )
                {
                    uint8_t accum = 0;
                    uint8_t mask = ~((0xFF) << (1<<factor));
                    // N=0  ->   right shift is always 0
                    // N=1  ->   right shift goes through 0, 4
                    // N=2  ->   right shift goes through 0, 2, 4, 6
                    // N=3  ->   right shift goes through 0, 1, 2, 3, 4, 5, 6, 7
                    data >>= ((page_offset & ((1<<factor) - 1))<<(3-factor));
                    for (uint8_t idx = 0; idx < 1<<(3-factor); idx++)
                    {
                         accum |= (((data>>idx) & 0x01) ? (mask<<(idx<<factor)) : 0);
                    }
                    data = accum;
                }
                for (uint8_t z=(1<<factor); z>0; z--)
                {
                    ssd1306_lcd.send_pixels1(data^s_ssd1306_invertByte);
                }
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for (i = 0; i < (char_info.spacing << factor); i++)
            ssd1306_lcd.send_pixels1(s_ssd1306_invertByte);
    }
    ssd1306_intf.stop();
    return j;
}

void         ssd1306_putPixel_delayed(uint8_t x, uint8_t y, uint8_t complete)
{
    static uint8_t lx = 0, ly = 0xFF;
    static uint8_t pixels = 0;
    if ((lx != x) || ((ly & 0xF8) != (y & 0xF8)) || (complete))
    {
        if (ly != 0xFF)
        {
            ssd1306_putPixels( lx, ly, pixels );
        }
        pixels = 0;
        ly = 0xFF;
    }
    if ( !complete )
    {
        pixels |= (1 << (y & 0x07));
        lx = x; ly = y;
    }
}

void         ssd1306_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    lcduint_t  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    lcduint_t  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    lcduint_t  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            ssd1306_swap_data(x1, x2, uint8_t);
            ssd1306_swap_data(y1, y2, uint8_t);
        }
        for(; y1<=y2; y1++)
        {
            err += dx;
            if (err >= dy)
            {
                 err -= dy;
                 x1 < x2 ? x1++: x1--;
            }
            ssd1306_putPixel_delayed( x1, y1, 0 );
        }
        ssd1306_putPixel_delayed( 0, 0, 1 );
    }
    else
    {
        if (x1 > x2)
        {
            ssd1306_swap_data(x1, x2, uint8_t);
            ssd1306_swap_data(y1, y2, uint8_t);
        }
        for(; x1<=x2; x1++)
        {
            err += dy;
            if (err >= dx)
            {
                 err -= dx;
                 if (y1 < y2) y1++; else y1--;
            }
            ssd1306_putPixel( x1, y1 );
        }
    }
}

void ssd1306_drawBufferFast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    uint8_t j;
    ssd1306_lcd.set_block(x, y >> 3, w);
    for(j=(h >> 3); j>0; j--)
    {
        ssd1306_lcd.send_pixels_buffer1(buf,w);
        buf+=w;
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

void gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)ssd1306_lcd.height) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= (lcdint_t)ssd1306_lcd.width)  return;
    if (y < 0)
    {
         buf += ((lcduint_t)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         buf += -x;
         w += x;
         x = 0;
    }
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ((lcduint_t)((lcduint_t)y + h) > (lcduint_t)ssd1306_lcd.height)
    {
         h = (lcduint_t)(ssd1306_lcd.height - (lcduint_t)y);
    }
    if ((lcduint_t)((lcduint_t)x + w) > (lcduint_t)ssd1306_lcd.width)
    {
         w = (lcduint_t)(ssd1306_lcd.width - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    ssd1306_lcd.set_block(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == (lcduint_t)(max_pages - 1) ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= (pgm_read_byte(buf) << offset);
            if ( complexFlag ) data |= (pgm_read_byte(buf - origin_width) >> (8 - offset));
            buf++;
            ssd1306_lcd.send_pixels1(s_ssd1306_invertByte^data);
        }
        buf += origin_width - w;
        complexFlag = offset;
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}


void ssd1306_setFont6x8(const uint8_t * progmemFont)
{
    s_font6x8 = progmemFont + 4;
}

#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template class NanoDisplayOps<1>;
template class NanoDisplayOps<8>;
template class NanoDisplayOps<16>;

template <uint8_t BPP>
void NanoDisplayOps<BPP>::putPixel(const NanoPoint &p)
{
    putPixel(p.x, p.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    drawHLine(x1, y1, x2);
    drawHLine(x1, y2, x2);
    drawVLine(x1, y1, y2);
    drawVLine(x2, y1, y2);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawRect(const NanoRect &rect)
{
    drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
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
void NanoDisplayOps<BPP>::drawLine(const NanoRect &rect)
{
    drawLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::fillRect(const NanoRect &rect)
{
    fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
uint8_t NanoDisplayOps<BPP>::printChar(uint8_t c)
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
       || ( (m_textMode & CANVAS_TEXT_WRAP) && (m_cursorX > ((lcdint_t)m_w - (lcdint_t)s_fixedFont.h.width)) ) )
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
size_t NanoDisplayOps<BPP>::write(uint8_t c)
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
void NanoDisplayOps<BPP>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
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

template <>
void NanoDisplayOps<1>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    setBlock(xpos, y, m_w - xpos);
    for(;;)
    {
        uint8_t ldata;
        if ( (x > m_w - s_fixedFont.h.width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (lcdint_t)( m_h >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == s_fixedFont.pages)
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            m_intf.stop();
            setBlock(xpos, y, m_w - xpos);
        }
        uint16_t unicode;
        do
        {
            unicode = ssd1306_unicode16FromUtf8(ch[j]);
            j++;
        } while ( unicode == SSD1306_MORE_CHARS_REQUIRED );
        SCharInfo char_info;
        ssd1306_getCharBitmap(unicode, &char_info);
        ldata = 0;
        x += char_info.width + char_info.spacing;
        if (char_info.height > page_offset * 8)
        {
            char_info.glyph += page_offset * char_info.width;
            for( i = char_info.width; i>0; i--)
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = pgm_read_byte(&char_info.glyph[0]);
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = pgm_read_byte(&char_info.glyph[0]);
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = pgm_read_byte(&char_info.glyph[1]);
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                m_intf.send(data^s_ssd1306_invertByte);
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for (i = 0; i < char_info.spacing; i++)
            m_intf.send(s_ssd1306_invertByte);
    }
    m_intf.stop();
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
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

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

template <uint8_t BPP>
static uint8_t getMaxScreenItems(NanoDisplayOps<BPP> &display)
{
    return (display.height() >> 3) - 2;
}

template <uint8_t BPP>
static uint8_t calculateScrollPosition(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems<BPP>(display) - 1)
    {
        return selection - getMaxScreenItems<BPP>(display) + 1;
    }
    return menu->scrollPosition;
}

template <uint8_t BPP>
static void drawMenuItem(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        display.negativeMode();
    }
    else
    {
        display.positiveMode();
    }
    display.setColor( 0xFFFF );
    display.printFixed(8, (index - menu->scrollPosition + 1)*8, menu->items[index], STYLE_NORMAL );
    display.positiveMode();
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::createMenu(SAppMenu *menu, const char **items, uint8_t count)
{
    menu->items = items;
    menu->count = count;
    menu->selection = 0;
    menu->oldSelection = 0;
    menu->scrollPosition = 0;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::showMenu( SAppMenu *menu)
{
    drawRect(4, 4, m_w - 5, m_h - 5);
    menu->scrollPosition = calculateScrollPosition<BPP>(*this, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, menu->scrollPosition + getMaxScreenItems<BPP>( *this )); i++)
    {
        drawMenuItem<BPP>(*this, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::updateMenu(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<BPP>( *this, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            clear();
            showMenu(menu);
        }
        else
        {
            drawMenuItem<BPP>( *this, menu, menu->oldSelection);
            drawMenuItem<BPP>( *this, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

template <uint8_t BPP>
uint8_t NanoDisplayOps<BPP>::menuSelection(SAppMenu *menu)
{
    return menu->selection;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::menuDown(SAppMenu *menu)
{
    if (menu->selection < menu->count - 1)
    {
        menu->selection++;
    }
    else
    {
        menu->selection = 0;
    }
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::menuUp(SAppMenu *menu)
{
    if (menu->selection > 0)
    {
        menu->selection--;
    }
    else
    {
        menu->selection = menu->count - 1;
    }
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<1> &canvas)
{
    drawBuffer1( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<8> &canvas)
{
    drawBuffer8( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<16> &canvas)
{
    drawBuffer16( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::negativeMode()
{
    s_ssd1306_invertByte = 0xFF;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::positiveMode()
{
    s_ssd1306_invertByte = 0x00;
}

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <>
void NanoDisplayOps<1>::putPixel(lcdint_t x, lcdint_t y)
{
    setBlock(x, y >> 3, 1);
    m_intf.send((1 << (y & 0x07))^s_ssd1306_invertByte);
    m_intf.stop();
}

template <>
void NanoDisplayOps<1>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    setBlock(x1, y1 >> 3, x2 - x1 + 1);
    for (uint8_t x = x1; x <= x2; x++)
    {
        m_intf.send((1 << (y1 & 0x07))^s_ssd1306_invertByte);
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<1>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    uint8_t topPage = y1 >> 3;
    uint8_t bottomPage = y2 >> 3;
    uint8_t height = y2-y1;
    uint8_t y;
    setBlock(x1, topPage, 1);
    if (topPage == bottomPage)
    {
        m_intf.send( ((0xFF >> (0x07 - height)) << (y1 & 0x07))^s_ssd1306_invertByte );
        m_intf.stop();
        return;
    }
    m_intf.send( (0xFF << (y1 & 0x07))^s_ssd1306_invertByte );
    for ( y = (topPage + 1); y <= (bottomPage - 1); y++)
    {
        nextPage();
        m_intf.send( 0xFF^s_ssd1306_invertByte );
    }
    nextPage();
    m_intf.send( (0xFF >> (0x07 - (y2 & 0x07)))^s_ssd1306_invertByte );
    m_intf.stop();
}

template <>
void NanoDisplayOps<1>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
}

template <>
void NanoDisplayOps<1>::clear()
{
    setBlock(0, 0, 0);
    for(uint8_t m=(m_h >> 3); m>0; m--)
    {
        for(uint8_t n=m_w; n>0; n--)
        {
            m_intf.send( s_ssd1306_invertByte );
        }
        nextPage();
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<1>::drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
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
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ((lcduint_t)((lcduint_t)y + h) > (lcduint_t)m_h)
    {
         h = (lcduint_t)(m_h - (lcduint_t)y);
    }
    if ((lcduint_t)((lcduint_t)x + w) > (lcduint_t)m_w)
    {
         w = (lcduint_t)(m_w - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    setBlock(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == (lcduint_t)(max_pages - 1) ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= ((pgm_read_byte(bitmap) << offset) & m_color);
            if ( complexFlag ) data |= ((pgm_read_byte(bitmap - origin_width) >> (8 - offset)) & m_color);
            bitmap++;
            m_intf.send(s_ssd1306_invertByte^data);
        }
        bitmap += origin_width - w;
        complexFlag = offset;
        nextPage();
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<1>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <>
void NanoDisplayOps<1>::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <>
void NanoDisplayOps<1>::drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= (lcdint_t)m_h) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= (lcdint_t)m_w)  return;
    if (y < 0)
    {
         buffer += ((lcduint_t)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         buffer += -x;
         w += x;
         x = 0;
    }
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ((lcduint_t)((lcduint_t)y + h) > (lcduint_t)m_h)
    {
         h = (lcduint_t)(m_h - (lcduint_t)y);
    }
    if ((lcduint_t)((lcduint_t)x + w) > (lcduint_t)m_w)
    {
         w = (lcduint_t)(m_w - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    setBlock(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == (lcduint_t)(max_pages - 1) ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= ((*buffer << offset) & m_color);
            if ( complexFlag ) data |= ((*(buffer - origin_width) >> (8 - offset)) & m_color);
            buffer++;
            m_intf.send(s_ssd1306_invertByte^data);
        }
        buffer += origin_width - w;
        complexFlag = offset;
        nextPage();
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<1>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <>
void NanoDisplayOps<1>::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <>
void NanoDisplayOps<1>::fill(uint16_t color)
{
    color ^= s_ssd1306_invertByte;
    setBlock(0, 0, 0);
    for(uint8_t m=(m_h >> 3); m>0; m--)
    {
        for(uint8_t n=m_w; n>0; n--)
        {
            m_intf.send(color);
        }
        nextPage();
    }
    m_intf.stop();
}
