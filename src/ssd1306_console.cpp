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

#include "ssd1306_console.h"

extern lcduint_t ssd1306_cursorX;
extern lcduint_t ssd1306_cursorY;
extern SFixedFontInfo s_fixedFont;

size_t ssd1306_consoleWriter(uint8_t ch)
{
    static uint8_t lcd_offset = 0;
    if (ch == '\r')
    {
        ssd1306_cursorX = 0;
        return 0;
    }
    else if ( (ssd1306_cursorX > ssd1306_lcd.width - s_fixedFont.h.width) || (ch == '\n') )
    {
        ssd1306_cursorX = 0;
        ssd1306_cursorY += s_fixedFont.h.height;
        uint8_t bottomScanLine = lcd_offset ? lcd_offset : ssd1306_lcd.height;
        if ( ssd1306_cursorY >= bottomScanLine )
        {
            if ( ssd1306_cursorY >= ssd1306_lcd.height )
            {
                ssd1306_cursorY -= (int8_t)ssd1306_lcd.height;
            }
            lcd_offset += s_fixedFont.h.height;
            if ( lcd_offset >= ssd1306_lcd.height )
            {
                lcd_offset -= (uint8_t)ssd1306_lcd.height;
            }
            ssd1306_setStartLine( lcd_offset );
        }
        ssd1306_clearBlock(0, ssd1306_cursorY >> 3, ssd1306_lcd.width, s_fixedFont.h.height);
        if (ch == '\n')
        {
            return 0;
        }
    }
    uint16_t unicode = ssd1306_unicode16FromUtf8(ch);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    ssd1306_drawBitmap( ssd1306_cursorX,
                        ssd1306_cursorY >> 3,
                        char_info.width,
                        char_info.height,
                        char_info.glyph );
    ssd1306_cursorX += char_info.width + char_info.spacing;
    return 1;
}

void Ssd1306Console::clear()
{
    ssd1306_clearScreen();
    setCursor(0,0);
}

void Ssd1306Console::setCursor(lcduint_t x, lcduint_t y)
{
    ssd1306_setCursor8(x,y);
}

