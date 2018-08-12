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

#include "nano_gfx.h"
#include "ssd1306.h"

extern const uint8_t *s_font6x8;
extern "C" SFixedFontInfo s_fixedFont;

#ifdef CONFIG_MULTIPLICATION_NOT_SUPPORTED
#define YADDR(y) (static_cast<uint16_t>((y) >> 3) << m_p)
#define BADDR(b) ((b) << m_p)
#else
#define YADDR(y) (static_cast<uint16_t>((y) >> 3) * m_w)
#define BADDR(b) ((b) * m_w)
#endif

void NanoCanvas::putPixel(uint8_t x, uint8_t y)
{
    if ((x>=m_w) || (y>=m_h)) return;
    if (m_invertByte)
    {
        m_bytes[YADDR(y) + x] &= ((1 << (y & 0x7))^m_invertByte);
    }
    else
    {
        m_bytes[YADDR(y) + x] |= (1 << (y & 0x7));
    }
};

void NanoCanvas::drawHLine(uint8_t x1, uint8_t y1, uint8_t x2)
{
    if (y1 >= m_h) return;
    if (x2 < x1) x1 = 0;
    if (x1 >= m_w) return;
    if (x2 >= m_w) x2 = m_w - 1;
    for(uint8_t x = x1; x<=x2; x++)
        m_bytes[YADDR(y1) + x] |= (1 << (y1 & 0x7));
};

void NanoCanvas::drawVLine(uint8_t x1, uint8_t y1, uint8_t y2)
{
    if (x1 >= m_w) return;
    if (y2 < y1) y1 = 0;
    if (y1 >= m_h) return;
    if (y2 >= m_h) y2 = m_h - 1;
    for(uint8_t y = y1; y<=y2; y++)
        m_bytes[YADDR(y) + x1] |= (1 << (y & 0x7));
};

void NanoCanvas::drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    drawHLine(x1, y1, x2);
    drawHLine(x1, y2, x2);
    drawVLine(x1, y1, y2);
    drawVLine(x2, y1, y2);
};


void NanoCanvas::fillRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t templ)
{
    templ ^= m_invertByte;
    if ((x1 < x2) && (x1 >= m_w)) return;
    if ((y1 < y2) && (y1 >= m_h)) return;
    if (x1 > x2) x1 = 0;
    if (y1 > y2) y1 = 0;
    if (x2 >= m_w) x2 = m_w -1;
    if (y2 >= m_h) y2 = m_h -1;
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
            m_bytes[BADDR(bank) + x] &= ~mask;
            m_bytes[BADDR(bank) + x] |= (templ & mask);
        }
    }
};


void NanoCanvas::clear()
{
    memset(m_bytes, m_invertByte, static_cast<uint16_t>(m_w) * (m_h >> 3));
}


void NanoCanvas::charF6x8(uint8_t x, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j, topMask, bottomMask;
    if (y>=m_h) return;
    j = 0;
    topMask = (0xFF >> (8 - (y & 0x7)));
    bottomMask = (0xFF << (y & 0x7));
    while(ch[j] != '\0')
    {
        uint8_t c = ch[j] - 32;
        uint8_t ldata = 0;
        for(i=0;i<6;i++)
        {
            if (x>=m_w) return;
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_font6x8[c*6+i]);
            }
            else if ( style == STYLE_BOLD )
            {
                data = pgm_read_byte(&s_font6x8[c*6+i]);
                uint8_t temp = data | ldata;
                ldata = data;
                data = temp;
            }
            else
            {
                data = pgm_read_byte(&s_font6x8[c*6+i + 1]);
                uint8_t temp = (data & 0xF0) | ldata;
                ldata = (data & 0x0F);
                data = temp;
            }
            m_bytes[YADDR(y) + x] &= topMask;
            m_bytes[YADDR(y) + x] |= (data << (y & 0x7));
            if (y+8 < m_h)
            {
                m_bytes[YADDR(y) + m_w + x] &= bottomMask;
                m_bytes[YADDR(y) + m_w + x] |= (data >> (8 - (y & 0x7)));
            }
            x++;
        }
        j++;
    }
}

void NanoCanvas::charF12x16(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t odd = 0;
    uint8_t x = xpos;
    uint8_t topMask, bottomMask;
    if ( y >= m_h ) return;
    topMask = (0xFF >> (8 - (y & 0x7)));
    bottomMask = (0xFF << (y & 0x7));
    for(;;)
    {
        if( ( x > m_w - 12 ) || ( ch[j] == '\0' ) )
        {
            x = xpos;
            y += 8;
            if (y > (m_h - 8))
            {
                break;
            }
            if (odd)
            {
                text_index = j;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            odd = !odd;
        }
        uint8_t c = ch[j] - 32;
        uint8_t ldata = 0;
        for( i=0; i<6; i++)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_font6x8[c*6+i]);
            }
            else if ( style == STYLE_BOLD )
            {
                data = pgm_read_byte(&s_font6x8[c*6+i]);
                uint8_t temp = data | ldata;
                ldata = data;
                data = temp;
            }
            else
            {
                data = pgm_read_byte(&s_font6x8[c*6+i + 1]);
                uint8_t temp = (data & 0xF0) | ldata;
                ldata = (data & 0x0F);
                data = temp;
            }
            if (odd) data >>= 4;
            data = ((data & 0x01) ? 0x03: 0x00) |
                   ((data & 0x02) ? 0x0C: 0x00) |
                   ((data & 0x04) ? 0x30: 0x00) |
                   ((data & 0x08) ? 0xC0: 0x00);

            for (uint8_t n=2; n>0; n--)
            {
                m_bytes[YADDR(y) + x] &= topMask;
                m_bytes[YADDR(y) + x] |= (data << (y & 0x7));
                if (y+8 < m_h)
                {
                    m_bytes[YADDR(y) + m_w + x] &= bottomMask;
                    m_bytes[YADDR(y) + m_w + x] |= (data >> (8 - (y & 0x7)));
                }
                x++;
            }
        }
        j++;
    }
}

void NanoCanvas::printFixed(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    uint8_t topMask, bottomMask;
    if ( y >= m_h ) return;
    topMask = (0xFF >> (8 - (y & 0x7)));
    bottomMask = (0xFF << (y & 0x7));
    for(;;)
    {
        if( ( x > m_w - s_fixedFont.h.width ) || ( ch[j] == '\0' ) )
        {
            x = xpos;
            y += 8;
            if (y > (m_h - 8))
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
        }
        uint8_t c = ch[j] - 32;
        if ( c > 224 )
        {
            c = 0;
        }
        uint8_t ldata = 0;
        uint16_t offset = (c * s_fixedFont.pages + page_offset) * s_fixedFont.h.width;
        for( i=0; i<s_fixedFont.h.width; i++)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset]);
                uint8_t temp = data | ldata;
                ldata = data;
                data = temp;
            }
            else
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset + 1]);
                uint8_t temp = (data & 0xF0) | ldata;
                ldata = (data & 0x0F);
                data = temp;
            }
            m_bytes[YADDR(y) + x] &= topMask;
            m_bytes[YADDR(y) + x] |= (data << (y & 0x7));
            if (y + 8 < m_h)
            {
                m_bytes[YADDR(y) + m_w + x] &= bottomMask;
                m_bytes[YADDR(y) + m_w + x] |= (data >> (8 - (y & 0x7)));
            }
            offset++;
            x++;
        }
        j++;
    }
}

void NanoCanvas::printFixed2x(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    uint8_t topMask, bottomMask;
    if ( y >= m_h ) return;
    topMask = (0xFF >> (8 - (y & 0x7)));
    bottomMask = (0xFF << (y & 0x7));
    for(;;)
    {
        if( ( x > m_w - (s_fixedFont.h.width<<1) ) || ( ch[j] == '\0' ) )
        {
            x = xpos;
            y += 8;
            if (y > (m_h - 8))
            {
                break;
            }
            page_offset++;
            if (page_offset == (s_fixedFont.pages<<1))
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
        }
        uint8_t c = ch[j] - 32;
        if ( c > 224 )
        {
            c = 0;
        }
        uint8_t ldata = 0;
        uint16_t offset = (c * s_fixedFont.pages + (page_offset >> 1)) * s_fixedFont.h.width;
        for( i=0; i<s_fixedFont.h.width; i++)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset]);
                uint8_t temp = data | ldata;
                ldata = data;
                data = temp;
            }
            else
            {
                data = pgm_read_byte(&s_fixedFont.primary_table[offset + 1]);
                uint8_t temp = (data & 0xF0) | ldata;
                ldata = (data & 0x0F);
                data = temp;
            }
            if (page_offset & 1) data >>= 4;
            data = ((data & 0x01) ? 0x03: 0x00) |
                   ((data & 0x02) ? 0x0C: 0x00) |
                   ((data & 0x04) ? 0x30: 0x00) |
                   ((data & 0x08) ? 0xC0: 0x00);

            for (uint8_t n=2; n>0; n--)
            {
                m_bytes[YADDR(y) + x] &= topMask;
                m_bytes[YADDR(y) + x] |= (data << (y & 0x7));
                if (y+8 < m_h)
                {
                    m_bytes[YADDR(y) + m_w + x] &= bottomMask;
                    m_bytes[YADDR(y) + m_w + x] |= (data >> (8 - (y & 0x7)));
                }
                x++;
            }
            offset++;
        }
        j++;
    }
}

void NanoCanvas::drawSpritePgm(uint8_t x, uint8_t y, const uint8_t sprite[])
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        if (x >= m_w) { x++; continue; }
        uint8_t d = pgm_read_byte(&sprite[i]);
        if (y < m_h)
            m_bytes[YADDR(y) + x] |= (d << (y & 0x7));
        if ((uint8_t)(y + 8) < m_h)
            m_bytes[YADDR((uint8_t)(y + 8)) + x] |= (d >> (8 - (y & 0x7)));
        x++;
    }
};


void NanoCanvas::drawBitmap(uint8_t startX, uint8_t startY, uint8_t w, uint8_t h, const uint8_t *buf)
{
    uint8_t x,y;
    for(y=0;y<h;y+=8)
    {
        for(x=0;x<w;x++)
        {
            uint8_t scrX = startX + x;
            if (scrX >= m_w) continue;
            uint8_t d = pgm_read_byte(&buf[x + static_cast<uint16_t>(y>>3) * w]);
            uint8_t scrY = y + startY;
            scrX = x + startX;
            if (scrY < m_h)
                m_bytes[YADDR(scrY) + scrX] |= (d << (scrY & 0x7));
            scrY+=8;
            if (scrY < m_h)
                m_bytes[YADDR(scrY) + scrX] |= (d >> (8 - (scrY & 0x7)));
        }
    }
}


void NanoCanvas::drawSprite(uint8_t x, uint8_t y, const uint8_t sprite[])
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        if (x>=m_w) { x++; continue; }
        uint8_t d = sprite[i];
        if (uint8_t(y) < m_h)
            m_bytes[YADDR(y) + x] |= (d << (y & 0x7));
        if ((uint8_t)(y+8) < m_h)
            m_bytes[YADDR((uint8_t)(y + 8)) + x] |= (d >> (8 - (y & 0x7)));
        x++;
    }
};

void NanoCanvas::drawSprite(SPRITE *sprite)
{
    uint8_t i;
    for(i = 0; i < sprite->w; i++)
    {
        if ((sprite->x + i) >= m_w) { continue; }
        uint8_t d = pgm_read_byte(&sprite->data[i]);
        if (sprite->y < m_h)
            m_bytes[YADDR(sprite->y) + sprite->x + i] |= (d << (sprite->y & 0x7));
        if (uint8_t(sprite->y + 8) < m_h)
            m_bytes[YADDR(uint8_t(sprite->y + 8)) + sprite->x + i] |= (d >> (8 - (sprite->y & 0x7)));
    }
}

void NanoCanvas::blt(uint8_t x, uint8_t y)
{
    ssd1306_drawBuffer(x, y, m_w, m_h, m_bytes);
}

void NanoCanvas::invert()
{
    for(uint16_t i=0; i< static_cast<uint16_t>(m_w) * (m_h >> 3); i++)
       m_bytes[i] = ~m_bytes[i];
}

void NanoCanvas::flipH()
{
   for (uint8_t y=0; y<(m_h>>3); y++)
       for (uint8_t x=0; x<m_w>>1; x++)
       {
           uint8_t temp = m_bytes[YADDR(y) + x];
           m_bytes[YADDR(y) + x] = m_bytes[YADDR(y) + m_w - x -1];
           m_bytes[YADDR(y) + m_w - x -1] = temp;
       }
}

////////////////////////////////////////////////////////////////////////////////
//                              SPRITE OBJECT
////////////////////////////////////////////////////////////////////////////////

void SPRITE::setPos(uint8_t x, uint8_t y)
{
    this->x = x;
    this->y = y;
}

void SPRITE::draw()
{
    ssd1306_drawSprite(this);
}

void SPRITE::eraseTrace()
{
    ssd1306_eraseTrace(this);
}

void SPRITE::erase()
{
    ssd1306_eraseSprite(this);
}
