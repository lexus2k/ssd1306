/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"

#include "lcd/ssd1331_commands.h"
#include "lcd/lcd_common.h"
#include "nano_engine/display.h"

//extern uint16_t ssd1306_color;
extern uint8_t s_ssd1306_invertByte;
//extern lcduint_t ssd1306_cursorX;
//extern lcduint_t ssd1306_cursorY;
extern SFixedFontInfo s_fixedFont;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern uint8_t g_ssd1306_unicode;
#endif

#if 0
void    ssd1306_setRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

void    ssd1306_setRgbColor8(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
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

void ssd1306_putColorPixel8(lcdint_t x, lcdint_t y, uint8_t color)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels8( color );
    ssd1306_intf.stop();
}

#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

//template class NanoDisplayOps<8>;

//template <>
//void NanoDisplayOps<8>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
//{
//    m_cursorX = xpos;
//    m_cursorY = y;
//    m_fontStyle = style;
//    print( ch );
//}

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <>
void NanoDisplayOps<8>::putPixel(lcdint_t x, lcdint_t y)
{
    setBlock(x, y, 0);
    m_intf.send( m_color );
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    setBlock(x1, y1, 0);
    while (x1 < x2)
    {
        m_intf.send( m_color );
        x1++;
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    setBlock(x1, y1, 1);
    while (y1<=y2)
    {
        m_intf.send( m_color );
        y1++;
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
    }
    setBlock(x1, y1, x2 - x1 + 1);
    uint16_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while (count--)
    {
        m_intf.send( m_color );
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::fill(uint16_t color)
{
    setBlock(0, 0, 0);
    uint32_t count = (uint32_t)m_w * (uint32_t)m_h;
    while (count--)
    {
        m_intf.send( color );
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::clear()
{
    fill( 0x00 );
}

template <>
void NanoDisplayOps<8>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint8_t blackColor = s_ssd1306_invertByte ? m_color : 0x00;
    uint8_t color = s_ssd1306_invertByte ? 0x00 : m_color;
    setBlock(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( data & bit )
                m_intf.send( color );
            else
                m_intf.send( blackColor );
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
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    setBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        m_intf.send( pgm_read_byte( bitmap ) );
        bitmap++;
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <>
void NanoDisplayOps<8>::drawBuffer1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    uint8_t bit = 1;
    uint8_t blackColor = s_ssd1306_invertByte ? m_color : 0x00;
    uint8_t color = s_ssd1306_invertByte ? 0x00 : m_color;
    setBlock(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = *buffer;
            if ( data & bit )
                m_intf.send( color );
            else
                m_intf.send( blackColor );
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
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    setBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        m_intf.send( *buffer );
        buffer++;
    }
    m_intf.stop();
}

template <>
void NanoDisplayOps<8>::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <>
void NanoDisplayOps<8>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    // TODO: fontstyle not supported
    // m_fontStyle = style;
    m_cursorX = xpos;
    m_cursorY = y;
    while (*ch)
    {
        write(*ch);
        ch++;
    }
}

