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
#include "hal/io.h"

#include "lcd/ssd1331_commands.h"
#include "lcd/lcd_common.h"

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
    ssd1306_commandStart();
    ssd1306_sendByte(SSD1331_DRAWLINE);
    ssd1306_sendByte(x1);
    ssd1306_sendByte(y1);
    ssd1306_sendByte(x2);
    ssd1306_sendByte(y2);
    ssd1306_sendByte( (color & 0x03) << 4 );
    ssd1306_sendByte( (color & 0x1C) << 2 );
    ssd1306_sendByte( (color & 0xE0) >> 2 );
    ssd1306_endTransmission();
}

void         ssd1331_fastDrawBuffer8(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *data)
{
    uint16_t count = w * h;
    ssd1306_setRamBlock(x, y, w);
    while (count--)
    {
        ssd1306_sendPixel8( *data );
        data++;
    }
    ssd1306_endTransmission();
}

void         ssd1331_fastDrawBuffer16(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *data)
{
    uint16_t count = (w * h) << 1;
    ssd1306_setRamBlock(x, y, w);
    while (count--)
    {
        ssd1306_sendByte( *data );
        data++;
    }
    ssd1306_endTransmission();
}
