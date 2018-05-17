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

#include "ssd1306.h"
#include "ssd1331_api.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"

#include "lcd/ssd1331_commands.h"
#include "lcd/lcd_common.h"

#define swap_data(a, b ,type)  { type t = a; a = b; b = t; }

extern uint16_t ssd1306_color;

void    ssd1331_setColor(uint16_t color)
{
    ssd1306_color = color;
}

void    ssd1331_setRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

void         ssd1331_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(SSD1331_DRAWLINE);
    ssd1306_intf.send(x1);
    ssd1306_intf.send(y1);
    ssd1306_intf.send(x2);
    ssd1306_intf.send(y2);
    ssd1306_intf.send( (color & 0x03) << 4 );
    ssd1306_intf.send( (color & 0x1C) << 2 );
    ssd1306_intf.send( (color & 0xE0) >> 2 );
    ssd1306_intf.stop();
}

void         ssd1331_drawBufferFast8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    uint16_t count = w * h;
    ssd1306_lcd.set_block(x, y, w);
    while (count--)
    {
        ssd1306_lcd.send_pixels8( *data );
        data++;
    }
    ssd1306_intf.stop();
}

void         ssd1331_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    uint16_t count = (w * h) << 1;
    ssd1306_lcd.set_block(x, y, w);
    while (count--)
    {
        ssd1306_intf.send( *data );
        data++;
    }
    ssd1306_intf.stop();
}

void ssd1331_putPixel8(lcdint_t x, lcdint_t y)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_intf.send( ssd1306_color );
    ssd1306_intf.stop();
}

void ssd1331_drawVLine8(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    ssd1306_lcd.set_block(x1, y1, 1);
    while (y1<=y2)
    {
        ssd1306_intf.send( ssd1306_color );
        y1++;
    }
    ssd1306_intf.stop();
}

void ssd1331_drawHLine8(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    ssd1306_lcd.set_block(x1, y1, 0);
    while (x1 < x2)
    {
        ssd1306_intf.send( ssd1306_color );
        x1++;
    }
    ssd1306_intf.stop();
}

void ssd1331_drawLine8(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    lcduint_t  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    lcduint_t  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    lcduint_t  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            swap_data(x1, x2, lcdint_t);
            swap_data(y1, y2, lcdint_t);
        }
        for(; y1<=y2; y1++)
        {
            err += dx;
            if (err >= dy)
            {
                 err -= dy;
                 x1 < x2 ? x1++: x1--;
            }
            ssd1331_putPixel8( x1, y1 );
        }
    }
    else
    {
        if (x1 > x2)
        {
            swap_data(x1, x2, lcdint_t);
            swap_data(y1, y2, lcdint_t);
        }
        for(; x1<=x2; x1++)
        {
            err += dy;
            if (err >= dx)
            {
                 err -= dx;
                 if (y1 < y2) y1++; else y1--;
            }
            ssd1331_putPixel8( x1, y1 );
        }
    }
}

void ssd1331_drawRect8(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    ssd1331_drawHLine8(x1,y1,x2);
    ssd1331_drawHLine8(x1,y2,x2);
    ssd1331_drawVLine8(x1,y1,y2);
    ssd1331_drawVLine8(x2,y1,y2);
}

void ssd1331_fillRect8(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        swap_data(x1, x2, lcdint_t);
    }
    ssd1306_lcd.set_block(x1, y1, x2 - x1 + 1);
    uint16_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while (count--)
    {
        ssd1306_intf.send( ssd1306_color );
    }
    ssd1306_intf.stop();
}

void ssd1331_drawMonoBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    ssd1306_lcd.set_block(xpos, ypos, w);
    while (h--)
    {
        uint8_t wx = w;
        while (wx--)
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( data & bit )
                ssd1306_intf.send( ssd1306_color );
            else
                ssd1306_intf.send( 0x00 );
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

void ssd1331_drawBitmap8(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    ssd1306_lcd.set_block(xpos, ypos, w);
    uint16_t count = (w) * (h);
    while (count--)
    {
        ssd1306_intf.send( pgm_read_byte( bitmap ) );
        bitmap++;
    }
    ssd1306_intf.stop();
}
