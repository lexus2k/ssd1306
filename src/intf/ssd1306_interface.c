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


void ssd1306_sendCommand(uint8_t command)
{
    ssd1306_startTransmission();
    ssd1306_sendByte(0x00);
    ssd1306_sendByte(command);
    ssd1306_endTransmission();
}


void ssd1306_dataStart(void)
{
    ssd1306_startTransmission();
    ssd1306_sendByte(0x40);
}

