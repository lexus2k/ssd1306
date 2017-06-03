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

#ifndef _SSD1306_PINS_H_
#define _SSD1306_PINS_H_

#include <Arduino.h>

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) 
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   PB3 // SCL, Pin 3 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   PB4 // SDA, Pin 4 on SSD1306 Board
    #endif
#else
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   5 // SCL, Pin A5 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 // SDA, Pin A4 on SSD1306 Board
    #endif
#endif

#endif /* _SSD1306_PINS_H_ */
