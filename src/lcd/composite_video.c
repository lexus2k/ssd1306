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

#include "composite_video.h"
#include "vga_commands.h"
#include "lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_hal/io.h"
#include "intf/vga/vga.h"

static const uint8_t PROGMEM s_composite128x64_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306,
    0x00,
#endif
    VGA_SET_RESOLUTION,128,64,1,
    VGA_DISPLAY_ON,
};

static uint8_t s_column = 0;
static uint8_t s_page = 0;

extern uint16_t ssd1306_color;

static void vga_set_block1(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_intf.send(0x00);
    ssd1306_intf.send(VGA_SET_BLOCK);
    ssd1306_intf.send(s_column);
    ssd1306_intf.send(rx);
    ssd1306_intf.send(s_page * 8);
    ssd1306_intf.stop();
    ssd1306_intf.start();
    ssd1306_intf.send(0x40);
}

static void vga_next_page1(void)
{
    ssd1306_intf.stop();
    vga_set_block1(s_column,s_page+1,0);
}

static void vga_set_block2(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    ssd1306_intf.start();
    ssd1306_intf.send(0x00);
    ssd1306_intf.send(VGA_SET_BLOCK); // set block
    ssd1306_intf.send(x);
    ssd1306_intf.send(rx);
    ssd1306_intf.send(y);
    ssd1306_intf.stop();
    ssd1306_intf.start();
    ssd1306_intf.send(0x40);
}

static void vga_next_page2(void)
{
}

#if 0

static void vga_send_pixels(uint8_t data)
{
    for (uint8_t i=8; i>0; i--)
    {
        if ( data & 0x01 )
        {
            ssd1306_intf.send( (uint8_t)ssd1306_color );
        }
        else
        {
            ssd1306_intf.send( 0B00000000 );
        }
        data >>= 1;
    }
}

#endif

static void vga_set_mode(lcd_mode_t mode)
{
    if (mode == LCD_MODE_NORMAL)
    {
        ssd1306_lcd.set_block = vga_set_block2;
        ssd1306_lcd.next_page = vga_next_page2;
    }
    else if (mode == LCD_MODE_SSD1306_COMPAT)
    {
        ssd1306_lcd.set_block = vga_set_block1;
        ssd1306_lcd.next_page = vga_next_page1;
    }
    ssd1306_intf.start();
    ssd1306_intf.send( 0x00 );
    ssd1306_intf.send( VGA_SET_MODE );
    ssd1306_intf.send( (uint8_t)mode );
    ssd1306_intf.stop();
    // empty for a while
}

void composite_video_128x64_mono_init(void)
{
    // init vga interface
    ssd1306_vgaInit();
    // init display
    ssd1306_lcd.type = LCD_TYPE_SSD1306;
    ssd1306_lcd.width = 128;
    ssd1306_lcd.height = 64;
    ssd1306_lcd.set_block = vga_set_block2;
    ssd1306_lcd.next_page = vga_next_page2;
    ssd1306_lcd.send_pixels1  = ssd1306_intf.send;
    ssd1306_lcd.send_pixels_buffer1 = ssd1306_intf.send_buffer;
    ssd1306_lcd.set_mode = vga_set_mode;
    ssd1306_configureI2cDisplay( s_composite128x64_initData, sizeof(s_composite128x64_initData));
}

