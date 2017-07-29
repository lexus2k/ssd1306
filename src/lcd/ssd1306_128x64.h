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
/**
 * @file ssd1306_128x64.h support for OLED 128x64 display
 */


#ifndef _SSD1306_128X64_H_
#define _SSD1306_128X64_H_

#include <Arduino.h>
#include <stdint.h>

/** OLED 128x64 initialization data */
extern const uint8_t PROGMEM s_oled128x64_initData[];
/** OLED 128x64 initialization data length */
extern const uint8_t s_oled128x64_dataLength;

// ----------------------------------------------------------------------------
#endif // _SSD1306_128X64_H_
