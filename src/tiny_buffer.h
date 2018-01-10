/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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

