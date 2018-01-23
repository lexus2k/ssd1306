/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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

#include "ssd1306_128x32.h"
#include "lcd_common.h"
#include "ssd1306_commands.h"
#include "intf/ssd1306_interface.h"
#include "i2c/ssd1306_i2c.h"
#include "hal/io.h"

#include <stdlib.h>

static const uint8_t PROGMEM s_oled128x32_initData[] =
{
    SSD1306_DISPLAYOFF, // display off
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,
    SSD1306_SETMULTIPLEX, 31,
    SSD1306_SETDISPLAYOFFSET, 0x00, // --no offset
    SSD1306_SETSTARTLINE,
    SSD1306_CHARGEPUMP, 0x14, // 0x10
    SSD1306_SEGREMAP | 0x01,  // Reverse mapping
    SSD1306_COMSCANDEC,
    SSD1306_SETCOMPINS, 0x02,
    SSD1306_SETCONTRAST, 0x7F, // contast value
    SSD1306_SETPRECHARGE, 0x22, // 0x1F
    SSD1306_SETVCOMDETECT, 0x40,
    SSD1306_MEMORYMODE, HORIZONTAL_ADDRESSING_MODE,
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    SSD1306_DISPLAYON 
};

static void ssd1306_setBlock(uint8_t x, uint8_t y, uint8_t w)
{
    ssd1306_commandStart();
    ssd1306_sendByte(SSD1306_COLUMNADDR);
    ssd1306_sendByte(x);
    ssd1306_sendByte(x + w - 1);
    ssd1306_sendByte(SSD1306_PAGEADDR);
    ssd1306_sendByte(y);
    ssd1306_sendByte((s_displayHeight >> 3) - 1);
    ssd1306_endTransmission();
}

static void ssd1306_nextPage()
{
}


/*
// We do not need this function, since ssd1306 library works in Horizontal Addressing mode
static void ssd1306_setPos(uint8_t x, uint8_t y)
{
    ssd1306_setBlock(0,0,s_displayWidth);
    ssd1306_commandStart();
    ssd1306_sendByte(SSD1306_SETPAGE | y);
    ssd1306_sendByte((x>>4) | SSD1306_SETHIGHCOLUMN);
    ssd1306_sendByte((x & 0x0f) | SSD1306_SETLOWCOLUMN);
    ssd1306_endTransmission();
}
*/

void    ssd1306_128x32_init()
{
    g_lcd_type = LCD_TYPE_SSD1306;
    s_displayHeight = 32;
    s_displayWidth = 128;
    ssd1306_setRamBlock = ssd1306_setBlock;
    ssd1306_nextRamPage = ssd1306_nextPage;
    ssd1306_sendPixels  = ssd1306_sendByte;
    for( uint8_t i=0; i < sizeof(s_oled128x32_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled128x32_initData[i]));
    }
}


void    ssd1306_128x32_i2c_init()
{
    ssd1306_i2cInit();
    ssd1306_128x32_init();
}
