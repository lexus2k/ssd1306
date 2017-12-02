/*
    Copyright (C) 2017 Alexey Dynda

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
/**
 * @file tiny_buffer.h Drawing in memory buffer
 */


#ifndef _TINY_BUFFER_H_
#define _TINY_BUFFER_H_

#include "nano_gfx.h"

/**
 * TinyBuffer represents object to work with double buffered display.
 * Easy to use:
 * TinyBuffer  lcd( SSD1306_128x64 );
 * void setup()
 * {
 *      lcd.beginI2C();
 *
 *      lcd.clear();
 *      lcd.charF6x8(0,0,"Hello");
 * }
 *
 */
class TinyBuffer: public NanoCanvas
{
public:
    /**
     * Creates new buffer object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h / 8.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    TinyBuffer(uint8_t w, uint8_t h, uint8_t *bytes): NanoCanvas( w, h, bytes ) {};

    /**
     * Writes single character to the display
     * @param ch - character to write
     */
//    virtual size_t write(uint8_t ch);
private:
    uint8_t      m_xpos = 0;

    uint8_t      m_ypos = 0;
};

#endif

