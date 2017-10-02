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

#include "ssd1306_128x64.h"
#include "intf/ssd1306_commands.h"


const uint8_t PROGMEM s_oled128x64_initData[] =
{
    SSD1306_DISPLAYOFF, // display off
    /* actually 2 commands below can be omitted */
    SSD1306_SETLOWCOLUMN, 
    SSD1306_SETHIGHCOLUMN, 
    /* normal page addressing mode */
    SSD1306_SETSTARTLINE,
    SSD1306_SETCONTRAST, 0x7F, // contast value
    SSD1306_SEGREMAP | 0x01,
    SSD1306_COMSCANDIR,
    SSD1306_NORMALDISPLAY,
    SSD1306_SETMULTIPLEX, 63,
    SSD1306_SETDISPLAYOFFSET, 0x00, // --no offset
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,
    SSD1306_SETPRECHARGE, 0xF1, // --switch vcc
    SSD1306_SETCOMPINS, 0x12, // --set divide ratio
    SSD1306_SETVCOMDETECT, 0x40, // --vcom detect
    SSD1306_MEMORYMODE, HORIZONTAL_ADDRESSING_MODE, // Page Addressing mode
    SSD1306_CHARGEPUMP, 0x14,
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    SSD1306_DISPLAYON 
};

const uint8_t s_oled128x64_dataLength = sizeof(s_oled128x64_initData);
