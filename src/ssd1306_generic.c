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

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "lcd/lcd_common.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_hal/io.h"
#include "nano_gfx_types.h"

enum
{
    SSD1306_OLD_FIXED_FORMAT = 0x00,
    SSD1306_NEW_FIXED_FORMAT = 0x01,
    SSD1306_NEW_FORMAT       = 0x02,
    SSD1306_SQUIX_FORMAT     = 0x03,
};

uint16_t ssd1306_color = 0xFFFF;
lcduint_t ssd1306_cursorX = 0;
lcduint_t ssd1306_cursorY = 0;
SFixedFontInfo s_fixedFont = {}; //{ { 0 }, 0 };
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
uint8_t g_ssd1306_unicode = 1;
#endif
static void (*s_ssd1306_getCharBitmap)(uint16_t unicode, SCharInfo *info) = NULL;

static const uint8_t *ssd1306_getCharGlyph(char ch);
static const uint8_t *ssd1306_getU16CharGlyph(uint16_t unicode);

lcduint_t      ssd1306_displayHeight()
{
    return ssd1306_lcd.height;
}

lcduint_t      ssd1306_displayWidth()
{
    return ssd1306_lcd.width;
}

void       ssd1306_setCursor(lcdint_t x, lcdint_t y)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
}

static const uint8_t * ssd1306_readUnicodeRecord(SUnicodeBlockRecord *r, const uint8_t *p)
{
    r->start_code =( pgm_read_byte(&p[0]) << 8) | (pgm_read_byte(&p[1]));
    r->count = pgm_read_byte(&p[2]);
    return (r->count > 0) ? (&p[3]): NULL;
}


void ssd1306_setSecondaryFont(const uint8_t * progmemUnicode)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    s_fixedFont.secondary_table = progmemUnicode;
    if (s_fixedFont.secondary_table != NULL)
    {
        s_fixedFont.secondary_table += sizeof(SFontHeaderRecord);
    }
#endif
}

void ssd1306_getCharBitmap(uint16_t unicode, SCharInfo *info)
{
    return s_ssd1306_getCharBitmap( unicode, info );
}

uint16_t ssd1306_unicode16FromUtf8(uint8_t ch)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    static uint16_t unicode = 0;
    ch &= 0x000000FF;
    if (!unicode)
    {
        if ( ch >= 0xc0 )
        {
            unicode = ch;
            return SSD1306_MORE_CHARS_REQUIRED;
        }
        return ch;
    }
    uint16_t code = ((unicode & 0x1f) << 6) | (ch & 0x3f);
    unicode = 0;
    return code;
#else
    return ch;
#endif
}

void ssd1306_enableUtf8Mode(void)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    g_ssd1306_unicode = 1;
#endif
}

void ssd1306_enableAsciiMode(void)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    g_ssd1306_unicode = 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// SECTION WITH FONT FORMATS
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
/// OLD FORMAT: 1.7.6 and below
/// OLD FORMAT is supported by old and latest versions of ssd1306 library

static const uint8_t *ssd1306_getCharGlyph(char ch)
{
     return &s_fixedFont.primary_table[ (ch - s_fixedFont.h.ascii_offset) *
                                        s_fixedFont.glyph_size +
                                        (s_fixedFont.h.type == 0x01 ? sizeof(SUnicodeBlockRecord) : 0) ];
}

#ifdef CONFIG_SSD1306_UNICODE_ENABLE
static const uint8_t *ssd1306_searchCharGlyph(const uint8_t * unicode_table, uint16_t unicode)
{
    SUnicodeBlockRecord r;
    const uint8_t *data = unicode_table;
    // looking for required unicode table
    while (1)
    {
        ssd1306_readUnicodeRecord( &r, data );
        if (r.count == 0)
        {
            break;
        }
        data += sizeof(SUnicodeBlockRecord);
        if ( ( unicode >= r.start_code) && ( unicode < (r.start_code + r.count) ) )
        {
            break;
        }
        data += r.count * s_fixedFont.glyph_size;
    }
    if (r.count == 0)
    {
        // Sorry, no glyph found for the specified character
        return NULL;
    }
    return &data[ (unicode - r.start_code) * s_fixedFont.glyph_size ];
}
#endif

static const uint8_t *ssd1306_getU16CharGlyph(uint16_t unicode)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    const uint8_t * glyph = NULL;
    if (g_ssd1306_unicode)
    {
        if ((unicode < 128) && (s_fixedFont.h.type == 0x00) && (s_fixedFont.primary_table != NULL))
        {
            return ssd1306_getCharGlyph(unicode);
        }
        if (s_fixedFont.primary_table)
        {
            glyph = ssd1306_searchCharGlyph( s_fixedFont.primary_table, unicode );
        }
        if (!glyph && s_fixedFont.secondary_table)
        {
            glyph = ssd1306_searchCharGlyph( s_fixedFont.secondary_table, unicode );
        }
        if (!glyph)
        {
            return ssd1306_getCharGlyph( s_fixedFont.h.ascii_offset );
        }
        return glyph;
    }
    else
#endif
    {
        return ssd1306_getCharGlyph(unicode);
    }
}

static void __ssd1306_oldFormatGetBitmap(uint16_t unicode, SCharInfo *info)
{
    if (info)
    {
        info->width = s_fixedFont.h.width;
        info->height = s_fixedFont.h.height;
        info->spacing = 0;
        info->glyph = ssd1306_getU16CharGlyph( unicode );
    }
}

void ssd1306_setFixedFont(const uint8_t * progmemFont)
{
    s_fixedFont.h.type   = pgm_read_byte( &progmemFont[0] );
    s_fixedFont.h.width  = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.primary_table = progmemFont + 4;
    s_ssd1306_getCharBitmap = __ssd1306_oldFormatGetBitmap;
    s_fixedFont.pages = (s_fixedFont.h.height + 7) >> 3;
    s_fixedFont.glyph_size = s_fixedFont.pages * s_fixedFont.h.width;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    s_fixedFont.secondary_table = NULL;
#endif
}

void ssd1306_setFixedFont_oldStyle(const uint8_t * progmemFont)
{
    s_fixedFont.h.type   = pgm_read_byte( &progmemFont[0] );
    s_fixedFont.h.width  = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.primary_table = progmemFont + 4;
    s_fixedFont.pages = (s_fixedFont.h.height + 7) >> 3;
    s_fixedFont.glyph_size = s_fixedFont.pages * s_fixedFont.h.width;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// NEW FORMAT: 1.7.8 and later
/// NEW FORMAT is supported only by latest versions of ssd1306 library

static void __ssd1306_newFormatGetBitmap(uint16_t unicode, SCharInfo *info)
{
    if (info)
    {
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
        uint8_t table_index = 0;
#endif
        const uint8_t *data = s_fixedFont.primary_table;
        while (data)
        {
            SUnicodeBlockRecord r;
            data = ssd1306_readUnicodeRecord( &r, data );
            if (!data)
            {
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
                if ( table_index == 0 )
                {
                    table_index++;
                    data = s_fixedFont.secondary_table;
                    continue;
                }
#endif
                break;
            }
            /* Check that unicode in the section being processed */
            if ( ( unicode < r.start_code) || ( unicode >= (r.start_code + r.count) ) )
            {
                // skip jump table
                data += r.count * 4;
                // skip block bitmap data
                data += ((pgm_read_byte(&data[0]) << 8) | (pgm_read_byte(&data[1]))) + 2;
                continue;
            }
            /* At this point data points to jump table (offset|offset|bytes|width) */
            unicode -= r.start_code;
            data += unicode * 4;
            uint16_t offset = (pgm_read_byte(&data[0]) << 8) | (pgm_read_byte(&data[1]));
            uint8_t glyph_width = pgm_read_byte(&data[2]);
            uint8_t glyph_height = pgm_read_byte(&data[3]);
            info->width = glyph_width;
            info->height = glyph_height;
            info->spacing = glyph_width ? 1 : (s_fixedFont.h.width >> 1);
            info->glyph = data + (r.count - unicode) * 4 + 2 + offset;
            break;
        }
        if (!info->glyph)
        {
            info->width = 0;
            info->height = 0;
            info->spacing = s_fixedFont.h.width >> 1;
            info->glyph = s_fixedFont.primary_table;
        }
    }
}

void ssd1306_setFreeFont(const uint8_t * progmemFont)
{
    s_fixedFont.h.type   = pgm_read_byte( &progmemFont[0] );
    s_fixedFont.h.width  = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.primary_table = progmemFont + 4;
    s_ssd1306_getCharBitmap = __ssd1306_newFormatGetBitmap;
    s_fixedFont.pages = (s_fixedFont.h.height + 7) >> 3;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    s_fixedFont.secondary_table = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// SQUIX FORMAT: 1.7.8 and later
/// SQUIX FORMAT is not fully supported. Use it at your own risk

static void __ssd1306_squixFormatGetBitmap(uint16_t unicode, SCharInfo *info)
{
    if (info)
    {
        const uint8_t *data = s_fixedFont.primary_table;
        /* Check that unicode in the section being processed */
        if ( !data || ( unicode < s_fixedFont.h.ascii_offset) || ( unicode >= (s_fixedFont.h.ascii_offset + s_fixedFont.count) ) )
        {
            info->width = 0;
            info->height = 0;
            info->spacing = s_fixedFont.h.width >> 1;
            info->glyph = s_fixedFont.primary_table;
            return;
        }
        /* At this point data points to jump table (offset|offset|bytes|width) */
        const uint8_t * bitmap_data = data + (uint16_t)s_fixedFont.count * 4;
        unicode -= s_fixedFont.h.ascii_offset;
        data += (unicode * 4);
        uint16_t offset = (pgm_read_byte(&data[0]) << 8) | pgm_read_byte(&data[1]);
        uint8_t glyph_bytes = pgm_read_byte(&data[2]);
//        uint8_t width = pgm_read_byte(&data[3]);
        info->width = glyph_bytes; //(glyph_bytes + s_fixedFont.pages - 1)  / s_fixedFont.pages;
        info->height = s_fixedFont.h.height / 2;
        info->spacing = 1;
//        uint8_t index=0;
        info->glyph = bitmap_data;
        if ( offset != 0xFFFF )
        {
             info->glyph += offset;
        }
    }
}

void ssd1306_setSquixFont(const uint8_t * progmemFont)
{
    s_fixedFont.h.type = SSD1306_SQUIX_FORMAT;
    s_fixedFont.h.width  = pgm_read_byte(&progmemFont[0]);
    s_fixedFont.h.height = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.count = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.primary_table = progmemFont + 4;
    s_ssd1306_getCharBitmap = __ssd1306_squixFormatGetBitmap;
    s_fixedFont.pages = (s_fixedFont.h.height + 7) >> 3;
    s_fixedFont.glyph_size = s_fixedFont.pages * s_fixedFont.h.width;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    s_fixedFont.secondary_table = NULL;
#endif
}

lcduint_t ssd1306_getTextSize(const char *text, lcduint_t *height)
{
    lcduint_t width = 0;
    while (*text)
    {
        if (*text == '\r' || *text == '\n')
        {
            text++;
            break;
        }
        uint16_t unicode = ssd1306_unicode16FromUtf8(*text);
        if (unicode == SSD1306_MORE_CHARS_REQUIRED)
        {
            text++;
            continue;
        }
        SCharInfo char_info;
        ssd1306_getCharBitmap(unicode, &char_info);
        width += char_info.width + char_info.spacing;
        if ( height ) *height = char_info.height;
        text++;
    }
    return width;
}
