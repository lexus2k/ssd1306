/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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

#include "lcd_pcd8544.h"
#include "lcd_common.h"
#include "pcd8544_commands.h"
#include "ssd1306_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

#if 0
static const uint8_t PROGMEM s_lcd84x48_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_PCD8544,
    0x00,
#endif
    PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION, // switch to extented commands
    PCD8544_SETVOP | 0x16,  // Set vop contrast
    PCD8544_SETTEMP,
    PCD8544_SETBIAS | 0x04, // Set bias mode
    PCD8544_FUNCTIONSET, // switch to basic commands
    PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL
};

static uint8_t s_column;
static uint8_t s_page;
static uint8_t s_width;

static void pcd8544_startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
#if 0
    s_width = w;
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    if (w == 1) ssd1306_intf.send( 0x22 ); else ssd1306_intf.send( 0x20 );
    ssd1306_intf.send(0x80 | x);
    ssd1306_intf.send(0x40 | y);
    ssd1306_spiDataMode(1);
#endif
}

static void pcd8544_nextBlock(void)
{
#if 0
    if ( s_width != 1)
    {
        ssd1306_intf.stop();
        pcd8544_startBlock(s_column, s_page+1, s_width);
    }
#endif
}

static void pcd8544_setMode(lcd_mode_t mode)
{
}

void pcd8544_84x48_init()
{
#if 0
    ssd1306_lcd.type = LCD_TYPE_PCD8544;
    ssd1306_lcd.width = 84;
    ssd1306_lcd.height = 48;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_lcd.set_block = pcd8544_startBlock;
    ssd1306_lcd.next_page = pcd8544_nextBlock;
    ssd1306_lcd.send_pixels1 = ssd1306_intf.send;
    ssd1306_lcd.send_pixels_buffer1 = ssd1306_intf.send_buffer;
    ssd1306_lcd.set_mode = pcd8544_setMode;

    for( uint8_t i=0; i<sizeof(s_lcd84x48_initData); i++)
    {
        ssd1306_intf.send(pgm_read_byte(&s_lcd84x48_initData[i]));
    }
    ssd1306_intf.stop();
#endif
}

void    pcd8544_84x48_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        ssd1306_resetController( rstPin, 1 );
    }
    ssd1306_spiInit(cesPin, dcPin);
    pcd8544_84x48_init();
}

#endif
