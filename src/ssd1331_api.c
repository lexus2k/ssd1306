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
#include "spi/ssd1306_spi.h"
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
    ssd1306_setRamBlock(x, y, w);
    while (count--)
    {
        ssd1306_sendPixel8( *data );
        data++;
    }
    ssd1306_intf.stop();
}

void         ssd1331_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    uint16_t count = (w * h) << 1;
    ssd1306_setRamBlock(x, y, w);
    while (count--)
    {
        ssd1306_intf.send( *data );
        data++;
    }
    ssd1306_intf.stop();
}
