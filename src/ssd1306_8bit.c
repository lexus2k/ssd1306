/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

#include "ssd1306_8bit.h"
#include "ssd1306_generic.h"
//#include "ssd1306_fonts.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"

#include "lcd/ssd1331_commands.h"
#include "lcd/lcd_common.h"

extern uint16_t ssd1306_color;
extern uint8_t s_ssd1306_invertByte;
extern lcduint_t ssd1306_cursorX;
extern lcduint_t ssd1306_cursorY;
extern SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern uint8_t g_ssd1306_unicode;
#endif

void    ssd1306_setColor(uint16_t color)
{
    ssd1306_color = color;
}

void    ssd1306_setRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

void    ssd1306_setRgbColor8(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

void ssd1306_drawMonoBuffer8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint8_t blackColor = s_ssd1306_invertByte ? ssd1306_color : 0x00;
    uint8_t color = s_ssd1306_invertByte ? 0x00 : ssd1306_color;
    ssd1306_lcd.set_block(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while (wx--)
        {
            uint8_t data = *bitmap;
            if ( data & bit )
                ssd1306_lcd.send_pixels8( color );
            else
                ssd1306_lcd.send_pixels8( blackColor );
            bitmap++;
        }
        bit <<= 1;
        if (bit == 0)
        {
            bit = 1;
        }
        else
        {
            bitmap -= w;
        }
    }
    ssd1306_intf.stop();
}

static void ssd1306_drawBufferPitch8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_lcd.set_block(x, y, w);
    while (h--)
    {
        lcduint_t line = w;
        while (line--)
        {
            ssd1306_lcd.send_pixels8( *data );
            data++;
        }
        data += pitch - w;
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBufferFast8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferPitch8( x, y, w, h, w, data );
}

void ssd1306_drawBufferEx8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_drawBufferPitch8( x, y, w, h, pitch, data );
}

void ssd1306_fillScreen8(uint8_t fill_Data)
{
    ssd1306_lcd.set_block(0, 0, 0);
    uint32_t count = (uint32_t)ssd1306_lcd.width * (uint32_t)ssd1306_lcd.height;
    while (count--)
    {
        ssd1306_lcd.send_pixels8( fill_Data );
    }
    ssd1306_intf.stop();
}

void ssd1306_clearScreen8(void)
{
    ssd1306_fillScreen8( 0x00 );
}

void ssd1306_putPixel8(lcdint_t x, lcdint_t y)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels8( ssd1306_color );
    ssd1306_intf.stop();
}

void ssd1306_putColorPixel8(lcdint_t x, lcdint_t y, uint8_t color)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels8( color );
    ssd1306_intf.stop();
}

void ssd1306_drawVLine8(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    ssd1306_lcd.set_block(x1, y1, 1);
    while (y1<=y2)
    {
        ssd1306_lcd.send_pixels8( ssd1306_color );
        y1++;
    }
    ssd1306_intf.stop();
}

void ssd1306_drawHLine8(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    ssd1306_lcd.set_block(x1, y1, 0);
    while (x1 < x2)
    {
        ssd1306_lcd.send_pixels8( ssd1306_color );
        x1++;
    }
    ssd1306_intf.stop();
}

void ssd1306_drawLine8(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    lcduint_t  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    lcduint_t  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    lcduint_t  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            ssd1306_swap_data(x1, x2, lcdint_t);
            ssd1306_swap_data(y1, y2, lcdint_t);
        }
        for(; y1<=y2; y1++)
        {
            err += dx;
            if (err >= dy)
            {
                 err -= dy;
                 x1 < x2 ? x1++: x1--;
            }
            ssd1306_putPixel8( x1, y1 );
        }
    }
    else
    {
        if (x1 > x2)
        {
            ssd1306_swap_data(x1, x2, lcdint_t);
            ssd1306_swap_data(y1, y2, lcdint_t);
        }
        for(; x1<=x2; x1++)
        {
            err += dy;
            if (err >= dx)
            {
                 err -= dx;
                 if (y1 < y2) y1++; else y1--;
            }
            ssd1306_putPixel8( x1, y1 );
        }
    }
}

void ssd1306_drawRect8(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    ssd1306_drawHLine8(x1,y1,x2);
    ssd1306_drawHLine8(x1,y2,x2);
    ssd1306_drawVLine8(x1,y1,y2);
    ssd1306_drawVLine8(x2,y1,y2);
}

void ssd1306_fillRect8(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
    }
    ssd1306_lcd.set_block(x1, y1, x2 - x1 + 1);
    uint16_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while (count--)
    {
        ssd1306_lcd.send_pixels8( ssd1306_color );
    }
    ssd1306_intf.stop();
}

void ssd1306_drawMonoBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint8_t blackColor = s_ssd1306_invertByte ? ssd1306_color : 0x00;
    uint8_t color = s_ssd1306_invertByte ? 0x00 : ssd1306_color;
    ssd1306_lcd.set_block(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( data & bit )
                ssd1306_lcd.send_pixels8( color );
            else
                ssd1306_lcd.send_pixels8( blackColor );
            bitmap++;
        }
        bit <<= 1;
        if ( bit == 0 )
        {
            bit = 1;
        }
        else
        {
            bitmap -= w;
        }
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    ssd1306_lcd.set_block(xpos, ypos, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        ssd1306_lcd.send_pixels8( pgm_read_byte( bitmap ) );
        bitmap++;
    }
    ssd1306_intf.stop();
}

void ssd1306_clearBlock8(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ssd1306_lcd.set_block(x, y, w);
    uint32_t count = w * h;
    while (count--)
    {
        ssd1306_lcd.send_pixels8( 0x00 );
    }
    ssd1306_intf.stop();
}

void ssd1306_setCursor8(lcduint_t x, lcduint_t y)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
}

void ssd1306_printChar8(uint8_t c)
{
    uint16_t unicode = ssd1306_unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    ssd1306_drawMonoBitmap8(ssd1306_cursorX,
                ssd1306_cursorY,
                char_info.width,
                char_info.height,
                char_info.glyph );
}

size_t ssd1306_write8(uint8_t ch)
{
    if (ch == '\r')
    {
        ssd1306_cursorX = 0;
        return 0;
    }
    SCharInfo char_info;
    uint8_t gotoNewLine = 1;
    if (ch != '\n')
    {
        uint16_t unicode;
        unicode = ssd1306_unicode16FromUtf8(ch);
        if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
        ssd1306_getCharBitmap(unicode, &char_info);
        gotoNewLine = (ssd1306_cursorX > (ssd1306_lcd.width - char_info.width));
    }
    if ( gotoNewLine )
    {
        ssd1306_cursorX = 0;
        ssd1306_cursorY += s_fixedFont.h.height;
        if ( ssd1306_cursorY > ssd1306_lcd.height - s_fixedFont.h.height )
        {
            ssd1306_cursorY = 0;
        }
        ssd1306_clearBlock8(0, ssd1306_cursorY, ssd1306_lcd.width, s_fixedFont.h.height);
        if (ch == '\n')
        {
            return 0;
        }
    }
    if ( 1 )
    {
        uint8_t color = ssd1306_color;
        ssd1306_color = s_ssd1306_invertByte ? color : 0x00;
        ssd1306_fillRect8( ssd1306_cursorX, ssd1306_cursorY,
                    ssd1306_cursorX + char_info.width + char_info.spacing - 1,
                    ssd1306_cursorY + s_fixedFont.h.height - 1 );
        ssd1306_color = color;
    }
    ssd1306_drawMonoBitmap8( ssd1306_cursorX,
                             ssd1306_cursorY,
                             char_info.width,
                             char_info.height,
                             char_info.glyph);
    ssd1306_cursorX += char_info.width + char_info.spacing;
    return 1;
}

size_t ssd1306_print8(const char ch[])
{
    size_t n = 0;
    while (*ch)
    {
        n += ssd1306_write8(*ch);
        ch++;
    }
    return n;
}

uint8_t ssd1306_printFixed8(lcdint_t x, lcdint_t y, const char *ch, EFontStyle style)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
    return ssd1306_print8(ch);
}


