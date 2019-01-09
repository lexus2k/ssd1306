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

#include "oled_template.h"
#include "lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

extern uint16_t ssd1306_color;

static const PROGMEM uint8_t s_oled_WxH_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_TEMPLATE,
    0x00,
#endif
    // Specify display initialization commands here
    // The commands should be in format, required by
    // ssd1306_configureI2cDisplay() or ssd1306_configureSpiDisplay()
    // functions. Refer to their documenation.
    0x00, 0x00,
};

/////////////   template functions below are for SPI display  ////////////
/////////////   in ssd1306 compatible mode                    ////////////

static uint8_t s_column;
static uint8_t s_page;

// The function must set block to draw data
static void template_setBlock_compat(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x00);   // Send column addr command to controller
    ssd1306_intf.send(x);
    ssd1306_intf.send(rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1));
    ssd1306_intf.send(0x00);   // Replace with row addr command to controller
    ssd1306_intf.send(y<<3);
    ssd1306_intf.send(((y<<3) + 7) < ssd1306_lcd.height ? ((y<<3) + 7) : (ssd1306_lcd.height - 1));
    ssd1306_spiDataMode(1);
}

static void template_nextPage_compat(void)
{
    ssd1306_intf.stop();
    template_setBlock_compat(s_column,s_page+1,0);
}

static void template_sendPixels(uint8_t data)
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

static void template_sendPixelsBuffer(const uint8_t *buffer, uint16_t len)
{
    while(len--)
    {
        template_sendPixels(*buffer);
        buffer++;
    }
}

/////////////   template functions below are for SPI display  ////////////
/////////////   in native/normal mode                         ////////////

static void template_setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x00);  // Send column address command here
    ssd1306_intf.send(x);
    ssd1306_intf.send(rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1));
    ssd1306_intf.send(0x00);  // Send row address command here
    ssd1306_intf.send(y);
    ssd1306_intf.send(ssd1306_lcd.height - 1);
    ssd1306_spiDataMode(1);
}

static void template_nextPage(void)
{
}

void    template_setMode(lcd_mode_t mode)
{
    if (mode == LCD_MODE_NORMAL)
    {
        ssd1306_lcd.set_block = template_setBlock;
        ssd1306_lcd.next_page = template_nextPage;
    }
    else if (mode == LCD_MODE_SSD1306_COMPAT)
    {
        ssd1306_lcd.set_block = template_setBlock_compat;
        ssd1306_lcd.next_page = template_nextPage_compat;
    }
    // Send command to update controller
//    ssd1306_intf.start();
//    ssd1306_spiDataMode(0);
//    ssd1306_intf.send( SSD1331_SEGREMAP );
//    ssd1306_intf.stop();
    return;
}

void    template_WxH_init()
{
    ssd1306_lcd.type = LCD_TYPE_CUSTOM;
    ssd1306_lcd.width = 96;  // specify width
    ssd1306_lcd.height = 64; // specify height
    // Set functions for compatible mode
    ssd1306_lcd.set_block = template_setBlock_compat;
    ssd1306_lcd.next_page = template_nextPage_compat;
    ssd1306_lcd.send_pixels1  = template_sendPixels;
    ssd1306_lcd.send_pixels_buffer1 = template_sendPixelsBuffer;
    // Set function for 8-bit mode
    ssd1306_lcd.send_pixels8 = ssd1306_intf.send;
    ssd1306_lcd.set_mode = template_setMode;
    // Use one of 2 functions for initialization below
    // Please, read help on this functions and read datasheet before you decide, which
    // one needs to be used. For example, ssd1331 is OK with ssd1306_configureI2cDisplay(),
    // while st7735 can be initialized only with ssd1306_configureSpiDisplay().
    ssd1306_configureI2cDisplay(s_oled_WxH_initData, sizeof(s_oled_WxH_initData));
//    ssd1306_configureSpiDisplay(s_oled_WxH_initData, sizeof(s_oled_WxH_initData));
}

void   template_WxH_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        ssd1306_resetController( rstPin, 10 );
    }
    ssd1306_spiInit(cesPin, dcPin);
    template_WxH_init();
}
