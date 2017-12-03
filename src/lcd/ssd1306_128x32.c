/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ssd1306_128x32.h"
#include "lcd_common.h"
#include "ssd1306_commands.h"
#include "intf/ssd1306_interface.h"
#include "i2c/ssd1306_i2c.h"

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
    // ssd1306 library doesn't use setRamPos intended for Page Addressing mode
    ssd1306_setRamPos = NULL; //ssd1306_setPos;
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
