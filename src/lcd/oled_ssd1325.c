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

#include "oled_ssd1325.h"
#include "lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

extern uint16_t ssd1306_color;

static const PROGMEM uint8_t s_oled_128x64_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1325,
    SDL_LCD_SSD1325_GENERIC,
#endif
    0xAE,        // OFF		                /* display off */
    0xB3, 0x91,	 // CLK
    0xA8, 0x3F,  // multiplex 64
    0xA2, 0x00,	 // Display offset
    0xA1, 0x00,	 // Start line
    0xAD, 0x02,	 // VCOMH
    0xA0, 0x40 | 0x10 | 0x04 | 0x02 | 0x01,	 // REMAP: vertical increment mode
    0x86,        // CURRENT
    0x81, 0x70,	 // CONTRAST
    0xB2, 0x51,	 // FREQ
    0xB1, 0x55,  // PHASE
    0xBC, 0x10,  // PRECHARGE
    0xBE, 0x1C,  // VCOMH voltage
    0xA4,        // NORMAL
};

/////////////   ssd1325 functions below are for SPI display  ////////////
/////////////   in ssd1306 compatible mode                   ////////////

//////////////////////// SSD1306 COMPATIBLE MODE ///////////////////////////////

static uint8_t __s_column;
static uint8_t __s_w;
static uint8_t __s_w2;
static uint8_t __s_page;
static uint8_t __s_leftPixel;
static uint8_t __s_pos;

static void set_block_compat(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    rx = rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1);
    __s_column = x;
    __s_page = y;
    __s_w = w;
    __s_w2 = rx - x + 1;
    __s_leftPixel = 0;
    __s_pos = __s_column;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x15);
    ssd1306_intf.send(x / 2);
    ssd1306_intf.send(rx / 2);
    ssd1306_intf.send(0x75);
    ssd1306_intf.send(y<<3);
    ssd1306_intf.send(((y<<3) + 7) < ssd1306_lcd.height ? ((y<<3) + 7) : (ssd1306_lcd.height - 1));
    ssd1306_spiDataMode(1);
}

static void next_page_compat(void)
{
    ssd1306_intf.stop();
    set_block_compat(__s_column,__s_page + 1, __s_w);
}

static void ssd1325_sendPixels(uint8_t data)
{
    if (!(__s_pos & 0x01))
    {
        __s_leftPixel = data;
        data = 0x00;
    }
    if ((__s_pos & 0x01) || (__s_pos == __s_column + __s_w2 - 1))
    {
        for (uint8_t i=8; i>0; i--)
        {
            uint8_t color = (__s_leftPixel & 0x01) ? (ssd1306_color & 0x0F) : 0;
            color |= (((data & 0x01) ? (ssd1306_color & 0x0F): 0) << 4);
            ssd1306_intf.send(color);
            data >>= 1;
            __s_leftPixel >>= 1;
        }
    }
    __s_pos++;
}

static void ssd1325_sendPixelsBuffer(const uint8_t *buffer, uint16_t len)
{
    while (len--)
    {
        ssd1325_sendPixels(*buffer);
        buffer++;
    }
}

//////////////////////// SSD1331 NATIVE MODE ///////////////////////////////////

CONTROLLER_NATIVE_SPI_BLOCK_8BIT_CMDS( 0x15, 0x75 );

/////////////   ssd1325 functions below are for SPI display  ////////////
/////////////   in native/normal mode                        ////////////

void    ssd1325_setMode(lcd_mode_t mode)
{
    if (mode == LCD_MODE_NORMAL)
    {
        ssd1306_lcd.set_block = set_block_native;
        ssd1306_lcd.next_page = next_page_native;
    }
    else if (mode == LCD_MODE_SSD1306_COMPAT )
    {
        ssd1306_lcd.set_block = set_block_compat;
        ssd1306_lcd.next_page = next_page_compat;
    }
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send( 0xA0 );
    ssd1306_intf.send( 0x10 | (mode == LCD_MODE_NORMAL ? 0x00 : 0x04) );
    ssd1306_intf.stop();
    return;
}

void    ssd1325_128x64_init()
{
    ssd1306_lcd.type = LCD_TYPE_CUSTOM;
    ssd1306_lcd.width = 128;  // specify width
    ssd1306_lcd.height = 64; // specify height
    // Set functions for compatible mode
    ssd1306_lcd.set_block = set_block_compat;
    ssd1306_lcd.next_page = next_page_compat;
    ssd1306_lcd.send_pixels1  = ssd1325_sendPixels;
    ssd1306_lcd.send_pixels_buffer1 = ssd1325_sendPixelsBuffer;
    // Set function for 8-bit mode
    ssd1306_lcd.send_pixels8 = ssd1306_intf.send;
    ssd1306_lcd.set_mode = ssd1325_setMode;
    // Use one of 2 functions for initialization below
    // Please, read help on this functions and read datasheet before you decide, which
    // one needs to be used. For example, ssd1331 is OK with ssd1306_configureI2cDisplay(),
    // while st7735 can be initialized only with ssd1306_configureSpiDisplay().
    ssd1306_configureI2cDisplay(s_oled_128x64_initData, sizeof(s_oled_128x64_initData));
}

void   ssd1325_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        ssd1306_resetController( rstPin, 10 );
    }
    ssd1306_spiInit(cesPin, dcPin);
    ssd1325_128x64_init();
}
