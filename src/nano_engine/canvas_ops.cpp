/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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

#include "canvas_ops.h"
#include "lcd/lcd_common.h"
#include "ssd1306.h"

extern const uint8_t *s_font6x8;
extern "C" SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern "C" uint8_t g_ssd1306_unicode;
#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

void NanoCanvasOpsInterface::putPixel(const NanoPoint &p)
{
    putPixel(p.x, p.y);
}

void NanoCanvasOpsInterface::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    drawHLine(x1, y1, x2);
    drawHLine(x1, y2, x2);
    drawVLine(x1, y1, y2);
    drawVLine(x2, y1, y2);
}

void NanoCanvasOpsInterface::drawRect(const NanoRect &rect)
{
    drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

void NanoCanvasOpsInterface::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    for (lcdint_t y = y1; y <= y2; y++) putPixel(x1, y);
}

void NanoCanvasOpsInterface::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    for (lcdint_t x = x1; x <= x2; x++) putPixel(x, y1);
}

void NanoCanvasOpsInterface::drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
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
            putPixel( x1, y1 );
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
            putPixel( x1, y1 );
        }
    }
}

void NanoCanvasOpsInterface::drawLine(const NanoRect &rect)
{
    drawLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}


void NanoCanvasOpsInterface::fillRect(const NanoRect &rect)
{
    fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}


uint8_t NanoCanvasOpsInterface::printChar(uint8_t c)
{
    uint16_t unicode = ssd1306_unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    uint8_t mode = m_textMode;
    for (uint8_t i = 0; i<(m_fontStyle == STYLE_BOLD ? 2: 1); i++)
    {
        drawBitmap1(m_cursorX + i,
                    m_cursorY,
                    char_info.width,
                    char_info.height,
                    char_info.glyph );
        m_textMode |= CANVAS_MODE_TRANSPARENT;
    }
    m_textMode = mode;
    m_cursorX += (lcdint_t)(char_info.width + char_info.spacing);
    if ( ( (m_textMode & CANVAS_TEXT_WRAP_LOCAL) && (m_cursorX > ((lcdint_t)m_w - (lcdint_t)s_fixedFont.h.width) ) )
       || ( (m_textMode & CANVAS_TEXT_WRAP) && (m_cursorX > ((lcdint_t)ssd1306_lcd.width - (lcdint_t)s_fixedFont.h.width)) ) )
    {
        m_cursorY += (lcdint_t)s_fixedFont.h.height;
        m_cursorX = 0;
        if ( (m_textMode & CANVAS_TEXT_WRAP_LOCAL) && (m_cursorY > ((lcdint_t)m_h - (lcdint_t)s_fixedFont.h.height)) )
        {
            m_cursorY = 0;
        }
    }
    return 1;
}


size_t NanoCanvasOpsInterface::write(uint8_t c)
{
    if (c == '\n')
    {
        m_cursorY += (lcdint_t)s_fixedFont.h.height;
        m_cursorX = 0;
    }
    else if (c == '\r')
    {
        // skip non-printed char
    }
    else
    {
        return printChar( c );
    }
    return 1;
}


void NanoCanvasOpsInterface::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    m_fontStyle = style;
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
}


void NanoCanvasOpsInterface::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    m_fontStyle = style;
    m_cursorX = xpos;
    m_cursorY = y;
    for (;;)
    {
        char c = pgm_read_byte(ch);
        if (!c) break;
        write(c);
        ch++;
    }
}

