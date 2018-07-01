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

///////////////////////////////////////////////////////////////////////////////
////// GENERIC FUNCTIONS APPLICABLE FOR ALL DISPLAY TYPES /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "lcd/lcd_common.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_hal/io.h"

// TODO: remove
#include "lcd/ssd1306_commands.h"

uint16_t ssd1306_color = 0xFFFF;
lcduint_t ssd1306_cursorX = 0;
lcduint_t ssd1306_cursorY = 0;
SFixedFontInfo s_fixedFont = { 0 };

uint8_t      ssd1306_displayHeight()
{
    return ssd1306_lcd.height;
}

uint8_t      ssd1306_displayWidth()
{
    return ssd1306_lcd.width;
}

void       ssd1306_setCursor(lcdint_t x, lcdint_t y)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
}

void ssd1306_setFixedFont(const uint8_t * progmemFont)
{
    s_fixedFont.width  = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.height = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.pages = (s_fixedFont.height + 7) >> 3;
    s_fixedFont.ascii_offset = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.data = progmemFont + 4;
}

void ssd1306_getCharBitmap(char ch, SCharInfo *info)
{
    if (info)
    {
        info->width = s_fixedFont.width;
        info->height = s_fixedFont.height;
        info->data = &s_fixedFont.data[ ch * s_fixedFont.pages * s_fixedFont.width ];
    }
}
