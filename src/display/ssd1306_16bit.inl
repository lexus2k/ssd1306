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

#include "ssd1306_hal/io.h"
#include "ssd1306_generic.h"

//extern uint16_t ssd1306_color;
extern uint8_t s_ssd1306_invertByte;
//extern lcduint_t ssd1306_cursorX;
//extern lcduint_t ssd1306_cursorY;
extern "C" SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern "C" uint8_t g_ssd1306_unicode;
#endif

#if 0
void    ssd1306_setRgbColor16(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR16(r,g,b);
}

static void ssd1306_drawBufferPitch16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_lcd.set_block(x, y, w);
    while (h--)
    {
        lcduint_t line = w << 1;
        while (line--)
        {
            ssd1306_intf.send( *data );
            data++;
        }
        data += pitch - (w << 1);
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferPitch16(x, y, w, h, w<<1, data);
}

void ssd1306_drawBufferEx16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_drawBufferPitch16( x, y, w, h, pitch, data );
}

void ssd1306_putColorPixel16(lcdint_t x, lcdint_t y, uint16_t color)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels16( color );
    ssd1306_intf.stop();
}

void ssd1306_drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    ssd1306_lcd.set_block(xpos, ypos, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        ssd1306_lcd.send_pixels16( (pgm_read_byte( &bitmap[0] ) << 8) | pgm_read_byte( &bitmap[1] ) );
        bitmap += 2;
    }
    ssd1306_intf.stop();
}

void ssd1306_clearBlock16(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ssd1306_lcd.set_block(x, y, w);
    uint32_t count = w * h;
    while (count--)
    {
        ssd1306_lcd.send_pixels16( 0x0000 );
    }
    ssd1306_intf.stop();
}

void ssd1306_setCursor16(lcduint_t x, lcduint_t y)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
}

void ssd1306_printChar16(uint8_t c)
{
    uint16_t unicode = ssd1306_unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    ssd1306_drawMonoBitmap16(ssd1306_cursorX,
                ssd1306_cursorY,
                char_info.width,
                char_info.height,
                char_info.glyph );
}

size_t ssd1306_write16(uint8_t ch)
{
    if (ch == '\r')
    {
        ssd1306_cursorX = 0;
        return 0;
    }
    else if ( (ssd1306_cursorX > ssd1306_lcd.width - s_fixedFont.h.width) || (ch == '\n') )
    {
        ssd1306_cursorX = 0;
        ssd1306_cursorY += s_fixedFont.h.height;
        if ( ssd1306_cursorY > ssd1306_lcd.height - s_fixedFont.h.height )
        {
            ssd1306_cursorY = 0;
        }
        ssd1306_clearBlock16(0, ssd1306_cursorY, ssd1306_lcd.width, s_fixedFont.h.height);
        if (ch == '\n')
        {
            return 0;
        }
    }
    uint16_t unicode = ssd1306_unicode16FromUtf8(ch);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    ssd1306_drawMonoBitmap16( ssd1306_cursorX,
                              ssd1306_cursorY,
                              char_info.width,
                              char_info.height,
                              char_info.glyph);
    ssd1306_cursorX += char_info.width + char_info.spacing;
    return 1;
}

size_t ssd1306_print16(const char ch[])
{
    size_t n = 0;
    while (*ch)
    {
        n += ssd1306_write16(*ch);
        ch++;
    }
    return n;
}

uint8_t ssd1306_printFixed16(lcdint_t x, lcdint_t y, const char *ch, EFontStyle style)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
    return ssd1306_print16(ch);
}

#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <class I>
void NanoDisplayOps16<I>::putPixel(lcdint_t x, lcdint_t y)
{
    this->m_intf.startBlock(x, y, 0);
    this->m_intf.send( this->m_color >> 8 );
    this->m_intf.send( this->m_color & 0xFF );
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    this->m_intf.startBlock(x1, y1, 0);
    while (x1 < x2)
    {
        this->m_intf.send( this->m_color >> 8 );
        this->m_intf.send( this->m_color & 0xFF );
        x1++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    this->m_intf.startBlock(x1, y1, 1);
    while (y1<=y2)
    {
        this->m_intf.send( this->m_color >> 8 );
        this->m_intf.send( this->m_color & 0xFF );
        y1++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
    }
    this->m_intf.startBlock(x1, y1, x2 - x1 + 1);
    uint16_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while (count--)
    {
        this->m_intf.send( this->m_color >> 8 );
        this->m_intf.send( this->m_color & 0xFF );
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::fill(uint16_t color)
{
    this->m_intf.startBlock(0, 0, 0);
    uint32_t count = (uint32_t)this->m_w * (uint32_t)this->m_h;
    while (count--)
    {
        this->m_intf.send( color >> 8 );
        this->m_intf.send( color & 0xFF );
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::clear()
{
    fill( 0x00 );
}

template <class I>
void NanoDisplayOps16<I>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint16_t blackColor = s_ssd1306_invertByte ? this->m_color : 0x0000;
    uint16_t color = s_ssd1306_invertByte ? 0x0000 : this->m_color;
    this->m_intf.startBlock(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( data & bit )
            {
                this->m_intf.send( color >> 8 );
                this->m_intf.send( color & 0xFF );
            }
            else
            {
                this->m_intf.send( blackColor >> 8 );
                this->m_intf.send( blackColor & 0xFF );
            }
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
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        uint16_t color = RGB8_TO_RGB16(pgm_read_byte(bitmap));
        this->m_intf.send( color >> 8 );
        this->m_intf.send( color & 0xFF );
        bitmap++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        this->m_intf.send( pgm_read_byte( &bitmap[0] ) );
        this->m_intf.send( pgm_read_byte( &bitmap[1] ) );
        bitmap += 2;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    uint8_t bit = 1;
    uint16_t blackColor = s_ssd1306_invertByte ? this->m_color : 0x0000;
    uint16_t color = s_ssd1306_invertByte ? 0x0000 : this->m_color;
    this->m_intf.startBlock(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = *buffer;
            if ( data & bit )
            {
                this->m_intf.send( color >> 8 );
                this->m_intf.send( color & 0xFF );
            }
            else
            {
                this->m_intf.send( blackColor >> 8 );
                this->m_intf.send( blackColor & 0xFF );
            }
            buffer++;
        }
        bit <<= 1;
        if ( bit == 0 )
        {
            bit = 1;
        }
        else
        {
            buffer -= w;
        }
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        uint16_t color = RGB8_TO_RGB16((*buffer));
        this->m_intf.send( color >> 8 );
        this->m_intf.send( color & 0xFF );
        buffer++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        this->m_intf.send( buffer[0] );
        this->m_intf.send( buffer[1] );
        buffer += 2;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    // TODO: fontstyle not supported
    // m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    while (*ch)
    {
        this->write(*ch);
        ch++;
    }
}


