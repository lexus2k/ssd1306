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

#ifndef _NANO_GFX_H_
#define _NANO_GFX_H_

#include "font6x8.h"

class NanoCanvas
{
public:
    /**
     * width can be of any value
     * height should be divided by 8
     */
    NanoCanvas(uint8_t w, uint8_t h, uint8_t *bytes)
    {
        m_w = w;
        m_h = h;
        m_p = 3;
        while (w >> (m_p+1)) { m_p++; };
        m_bytes = bytes;
        clear();
    };

    /**
     *  
     */
    void drawPixel(uint8_t x, uint8_t y);

    void drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

    void drawHLine(uint8_t x1, uint8_t y1, uint8_t x2);

    void drawVLine(uint8_t x1, uint8_t y1, uint8_t y2);

    void fillRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t templ);

    void clear();
    
    void char_f6x8(uint8_t x, uint8_t y, const char ch[]);

    /**
     * Sprite is small image 8x8, sprite doesn't change background
     * Reads sprite from Program memory
     */
    void drawSpritePgm(uint8_t x, uint8_t y, const uint8_t sprite[]);

    /**
     * Sprite is small image 8x8, sprite doesn't change background
     * Reads sprite from SRAM memory
     */
    void drawSprite(uint8_t x, uint8_t y, const uint8_t sprite[]);

    inline uint8_t width() { return m_w; };
    inline uint8_t height() { return m_h; };
    inline uint8_t *buffer() { return m_bytes; };

private:
    uint8_t m_w;
    uint8_t m_p;
    uint8_t m_h;
    uint8_t *m_bytes;
};

#endif

