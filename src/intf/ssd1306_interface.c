/*
    Copyright (C) 2016-2017 Alexey Dynda

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

#include "ssd1306_interface.h"

#include "lcd/ssd1306_commands.h"
#include "lcd/lcd_common.h"
#include <stddef.h>

/**
 * Starts communication with SSD1306 display.
 */
void  (*ssd1306_startTransmission)() = NULL;

/**
 * Ends communication with SSD1306 display.
 */
void  (*ssd1306_endTransmission)() = NULL;

/**
 * Sends byte to SSD1306 device
 * @param data - byte to send
 */
void  (*ssd1306_sendByte)(uint8_t data) = NULL;

void (*ssd1306_commandStart)() = NULL;

void (*ssd1306_dataStart)() = NULL;

void ssd1306_sendCommand(uint8_t command)
{
    ssd1306_commandStart();
    ssd1306_sendByte(command);
    ssd1306_endTransmission();
}

void ssd1306_sendData(uint8_t data)
{
    ssd1306_dataStart();
    ssd1306_sendByte( data );
    ssd1306_endTransmission();
}

void ssd1306_setRamBlock(uint8_t x, uint8_t y, uint8_t w)
{
    if ( g_lcd_type == LCD_TYPE_SSD1306 )
    {
        ssd1306_commandStart();
        ssd1306_sendByte(SSD1306_COLUMNADDR);
        ssd1306_sendByte(x);
        ssd1306_sendByte(x + w - 1);
        ssd1306_sendByte(SSD1306_PAGEADDR);
        ssd1306_sendByte(y);
        ssd1306_sendByte((ssd1306_displayHeight() >> 3) - 1);
//        ssd1306_sendByte(SSD1306_SETPAGE | y);
//        ssd1306_sendByte((x>>4) | SSD1306_SETHIGHCOLUMN);
//        ssd1306_sendByte((x & 0x0f) | SSD1306_SETLOWCOLUMN);
        ssd1306_endTransmission();
    }
    else if ( g_lcd_type == LCD_TYPE_PCD8544 )
    {
        ssd1306_commandStart();
        if (w == 1) ssd1306_sendByte( 0x22 ); else ssd1306_sendByte( 0x20 );
        ssd1306_sendByte(0x80 | x);
        ssd1306_sendByte(0x40 | y);
        ssd1306_endTransmission();
    }
}

void ssd1306_setPos(uint8_t x, uint8_t y)
{
    if ( g_lcd_type == LCD_TYPE_SSD1306 )
    {
        ssd1306_commandStart();
        ssd1306_sendByte(SSD1306_SETPAGE | y);
        ssd1306_sendByte((x>>4) | SSD1306_SETHIGHCOLUMN);
        ssd1306_sendByte((x & 0x0f) | SSD1306_SETLOWCOLUMN);
        ssd1306_endTransmission();
    }
    else if ( g_lcd_type == LCD_TYPE_PCD8544 )
    {
        ssd1306_commandStart();
        ssd1306_sendByte(0x80 | x);
        ssd1306_sendByte(0x40 | y);
        ssd1306_endTransmission();
    }
}
