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

#include "tiny_ssd1306.h"

/* Font width in pixels */
static const uint8_t FIXED_FONT_WIDTH = 6;
/* Font height in blocks */
static const uint8_t FIXED_FONT_HEIGHT = 1;

size_t TinySSD1306::write(uint8_t ch)
{
    const char str[2] = { static_cast<char>(ch), '\0' };
    if (ch == '\r')
    {
        m_xpos = 0;
    }
    if ( (m_xpos > width() - FIXED_FONT_WIDTH) || (ch == '\n') )
    {
        m_xpos = 0;
        m_ypos += FIXED_FONT_HEIGHT;
    }
    if ( m_ypos > (height() >> 3) - FIXED_FONT_HEIGHT )
    {
        m_ypos = 0;
    }
    ssd1306_printFixed( m_xpos, m_ypos, str, STYLE_NORMAL );
    m_xpos += FIXED_FONT_WIDTH;
}

void TinySSD1306::clear()
{
    m_xpos = 0;
    m_ypos = 0;
    ssd1306_clearScreen();
};
