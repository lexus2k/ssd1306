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

void  (*ssd1306_startTransmission)() = NULL;
void  (*ssd1306_endTransmission)() = NULL;
void  (*ssd1306_sendByte)(uint8_t data) = NULL;
void  (*ssd1306_commandStart)() = NULL;
void  (*ssd1306_dataStart)() = NULL;

void  (*ssd1306_setRamBlock)(uint8_t x, uint8_t y, uint8_t w) = NULL;
void  (*ssd1306_nextRamPage)() = NULL;
void  (*ssd1306_setRamPos)(uint8_t x, uint8_t y) = NULL;

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
