/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nano_gfx.h"
#include "ssd1306.h"

extern const uint8_t *s_font6x8;

#define YADDR(y) (((y) >> 3) << m_p)
#define BADDR(b) ((b) << m_p)

void NanoCanvas::putPixel(uint8_t x, uint8_t y)
{
    if ((x>=m_w) || (y>=m_h)) return;
    if (m_inverseByte)
    {
        m_bytes[YADDR(y) + x] &= ((1 << (y & 0x7))^m_inverseByte);
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
    templ ^= m_inverseByte;
    if ((x1 < x2) && (x1 >= m_w)) return;
    if ((y1 < y2) && (y1 >= m_h)) return;
    if (x1 > x2) x1 = 0;
    if (y1 > y2) y1 = 0;
    if (x2 >= m_w) x2 = m_w -1;
    if (y2 >= m_h) y2 = m_h -1;
    uint8_t bank1 = (y1 >> 3);
    uint8_t bank2 = (y2 >> 3);
    for (uint8_t bank = bank1; bank2<=(y2 >> 3); bank++)
    {
        uint8_t mask = 0xFF;
        if (bank1 == bank2)
        {
            mask = (mask >> ((y1 & 7) + 7 - (y2 & 7))) << (7 - (y2 & 7));
        }
        else if (bank1 == bank)
        {
            mask = (mask >> (y1 & 7));
        }
        else if (bank2 == bank)
        {
            mask = (mask << (7 - (y2 & 7)));
        }
        for (uint8_t x=x1; x<x2; x++)
        {
            m_bytes[BADDR(bank) + x] &= ~mask;
            m_bytes[BADDR(bank) + x] |= (templ & mask);
        }
    }
};


void NanoCanvas::clear()
{
    for(uint16_t i=0; i<m_w* (m_h >> 3); i++)
    {
       m_bytes[i] = m_inverseByte;
    }
}


void NanoCanvas::charF6x8(uint8_t x, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j;
    if (y>=m_h) return;
    j = 0;
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
            m_bytes[YADDR(y) + x] |= (data << (y & 0x7));
            if (y+8 < m_h)
                m_bytes[YADDR(y) + m_w + x] |= (data >> (8 - (y & 0x7)));
            x++;
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
    for(uint16_t i=0; i<m_w* (m_h >> 3); i++)
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
