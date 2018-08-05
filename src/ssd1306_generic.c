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
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
uint8_t g_ssd1306_unicode = 1;
#endif

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
    s_fixedFont.h.type   = pgm_read_byte(&progmemFont[0]);
    s_fixedFont.h.width  = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.pages = (s_fixedFont.h.height + 7) >> 3;
    s_fixedFont.data = progmemFont + sizeof(SFontHeaderRecord);
    if (s_fixedFont.h.type == 0x01)
    {
        s_fixedFont.data += sizeof(SUnicodeBlockRecord);
    }
}

void ssd1306_getCharBitmap(char ch, SCharInfo *info)
{
    if (info)
    {
        info->width = s_fixedFont.h.width;
        info->height = s_fixedFont.h.height;
        info->data = ssd1306_getCharGlyph( ch );
    }
}

const uint8_t *ssd1306_getCharGlyph(char ch)
{
     return &s_fixedFont.data[ (ch - s_fixedFont.h.ascii_offset) *
                                s_fixedFont.pages * s_fixedFont.h.width ];
}

static void ssd1306_read_unicode_record(SUnicodeBlockRecord *r, const uint8_t *p)
{
    r->start_code = pgm_read_byte(&p[0]) | (pgm_read_byte(&p[1]) << 8);
    r->count = pgm_read_byte(&p[2]) | (pgm_read_byte(&p[3]) << 8);
    r->size = 0;
    if (r->count != 0)
    {
        r->size = pgm_read_byte(&p[4]) | (pgm_read_byte(&p[5]) << 8) |
                  (pgm_read_byte(&p[6]) << 16) | (pgm_read_byte(&p[7]) << 24);
        if (!r->size)
        {
            r->size = r->count * s_fixedFont.pages * s_fixedFont.h.width;
        }
    }
}

const uint8_t *ssd1306_getU16CharGlyph(uint16_t unicode)
{
    const uint8_t *data = s_fixedFont.data;
    SUnicodeBlockRecord r;
    if (s_fixedFont.h.type == 0x00)
    {
        r.start_code = s_fixedFont.h.ascii_offset;
        r.count = 128 - s_fixedFont.h.ascii_offset;
        r.size = s_fixedFont.pages * s_fixedFont.h.width * r.count;
    }
    else
    {
        ssd1306_read_unicode_record(&r, data - sizeof(SFontHeaderRecord));
    }
    // looking for required unicode table
    while ( ( unicode < r.start_code) || ( unicode >= (r.start_code + r.count) ) )
    {
        if (r.count == 0) break;
        data += r.size;
        ssd1306_read_unicode_record( &r, data );
        data += sizeof(SUnicodeBlockRecord);
    }
    if (r.count == 0)
    {
        // Sorry, no glyph found for the specified character
        return s_fixedFont.data;
    }
    return &data[ (unicode - r.start_code) * s_fixedFont.pages * s_fixedFont.h.width ];
}

uint16_t get_unicode16_from_utf8(uint8_t ch)
{
    static uint16_t unicode = 0;
    ch &= 0x000000FF;
    if (!unicode)
    {
        if ( ch >= 0xc0 )
        {
            unicode = ch;
            return 0xffff;
        }
        return ch;
    }
    uint16_t code = ((unicode & 0x1f) << 6) | (ch & 0x3f);
    unicode = 0;
    return code;
}

void ssd1306_enableUtf8Mode()
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    g_ssd1306_unicode = 1;
#endif
}

void ssd1306_enableAsciiMode()
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    g_ssd1306_unicode = 0;
#endif
}
