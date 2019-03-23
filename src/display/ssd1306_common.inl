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

#include "ssd1306_hal/io.h"
#include "ssd1306_generic.h"

//const uint8_t *s_font6x8 = &ssd1306xled_font6x8[4];
//extern lcduint_t ssd1306_cursorX;
//extern lcduint_t ssd1306_cursorY;
extern "C" SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern "C" uint8_t g_ssd1306_unicode;
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

template <class O, class I>
void NanoDisplayOps<O,I>::putPixel(const NanoPoint &p)
{
    this->putPixel(p.x, p.y);
}

template <class O, class I>
void NanoDisplayOps<O,I>::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    this->drawHLine(x1, y1, x2);
    this->drawHLine(x1, y2, x2);
    this->drawVLine(x1, y1, y2);
    this->drawVLine(x2, y1, y2);
}

template <class O, class I>
void NanoDisplayOps<O,I>::drawRect(const NanoRect &rect)
{
    this->drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <class O, class I>
void NanoDisplayOps<O,I>::drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
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
            this->putPixel( x1, y1 );
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
            this->putPixel( x1, y1 );
        }
    }
}

template <class O, class I>
void NanoDisplayOps<O,I>::drawLine(const NanoRect &rect)
{
    this->drawLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <class O, class I>
void NanoDisplayOps<O,I>::fillRect(const NanoRect &rect)
{
    this->fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <class O, class I>
uint8_t NanoDisplayOps<O,I>::printChar(uint8_t c)
{
    uint16_t unicode = ssd1306_unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    uint8_t mode = this->m_textMode;
    for (uint8_t i = 0; i<(this->m_fontStyle == STYLE_BOLD ? 2: 1); i++)
    {
        this->drawBitmap1(this->m_cursorX + i,
                    this->m_cursorY,
                    char_info.width,
                    char_info.height,
                    char_info.glyph );
        this->m_textMode |= CANVAS_MODE_TRANSPARENT;
    }
    this->m_textMode = mode;
    this->m_cursorX += (lcdint_t)(char_info.width + char_info.spacing);
    if ( ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) && (this->m_cursorX > ((lcdint_t)this->m_w - (lcdint_t)s_fixedFont.h.width) ) )
       || ( (this->m_textMode & CANVAS_TEXT_WRAP) && (this->m_cursorX > ((lcdint_t)this->m_w - (lcdint_t)s_fixedFont.h.width)) ) )
    {
        this->m_cursorY += (lcdint_t)s_fixedFont.h.height;
        this->m_cursorX = 0;
        if ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) && (this->m_cursorY > ((lcdint_t)this->m_h - (lcdint_t)s_fixedFont.h.height)) )
        {
            this->m_cursorY = 0;
        }
    }
    return 1;
}

template <class O, class I>
size_t NanoDisplayOps<O,I>::write(uint8_t c)
{
    if (c == '\n')
    {
        this->m_cursorY += (lcdint_t)s_fixedFont.h.height;
        this->m_cursorX = 0;
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

/*
template <class O, class I>
void NanoDisplayOps<O,I>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    this->m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    while (*ch)
    {
        this->write(*ch);
        ch++;
    }
}*/

template <class O, class I>
void NanoDisplayOps<O,I>::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    this->m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    for (;;)
    {
        char c = pgm_read_byte(ch);
        if (!c) break;
        this->write(c);
        ch++;
    }
}

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

template <class O, class I>
static uint8_t getMaxScreenItems(NanoDisplayOps<O,I> &display)
{
    return (display.height() >> 3) - 2;
}

template <class O, class I>
static uint8_t calculateScrollPosition(NanoDisplayOps<O,I> &display, SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems<O,I>(display) - 1)
    {
        return selection - getMaxScreenItems<O,I>(display) + 1;
    }
    return menu->scrollPosition;
}

template <class O, class I>
static void drawMenuItem(NanoDisplayOps<O,I> &display, SAppMenu *menu, uint8_t index)
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

template <class O, class I>
void NanoDisplayOps<O,I>::createMenu(SAppMenu *menu, const char **items, uint8_t count)
{
    menu->items = items;
    menu->count = count;
    menu->selection = 0;
    menu->oldSelection = 0;
    menu->scrollPosition = 0;
}

template <class O, class I>
void NanoDisplayOps<O,I>::showMenu( SAppMenu *menu)
{
    drawRect(4, 4, this->m_w - 5, this->m_h - 5);
    menu->scrollPosition = calculateScrollPosition<O,I>(*this, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, (menu->scrollPosition + getMaxScreenItems<O,I>( *this ))); i++)
    {
        drawMenuItem<O,I>(*this, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <class O, class I>
void NanoDisplayOps<O,I>::updateMenu(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<O,I>( *this, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            this->clear();
            showMenu(menu);
        }
        else
        {
            drawMenuItem<O,I>( *this, menu, menu->oldSelection);
            drawMenuItem<O,I>( *this, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

template <class O, class I>
uint8_t NanoDisplayOps<O,I>::menuSelection(SAppMenu *menu)
{
    return menu->selection;
}

template <class O, class I>
void NanoDisplayOps<O,I>::menuDown(SAppMenu *menu)
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

template <class O, class I>
void NanoDisplayOps<O,I>::menuUp(SAppMenu *menu)
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

template <class O, class I>
void NanoDisplayOps<O,I>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<1> &canvas)
{
    this->drawBuffer1( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <class O, class I>
void NanoDisplayOps<O,I>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<8> &canvas)
{
    this->drawBuffer8( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <class O, class I>
void NanoDisplayOps<O,I>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<16> &canvas)
{
    this->drawBuffer16( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

