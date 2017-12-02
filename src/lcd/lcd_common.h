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
 * @file lcd_common.h global lcd settings
 */

#ifndef _LCD_COMMON_H_
#define _LCD_COMMON_H_

#include <stdint.h>

#if defined(ESP8266) || defined(ESP32) || defined(ESP31B)
    #include <pgmspace.h>
#else
    #include <avr/pgmspace.h>
#endif

enum
{
    /** Default type of LCD display: ssd1306 oled */
    LCD_TYPE_SSD1306,
    /** Experimental type of LCD display: pcd8544 led */
    LCD_TYPE_PCD8544,
    /** Support for sh1106 OLED display */
    LCD_TYPE_SH1106,
    /** User LCD type */
    LCD_TYPE_CUSTOM,
};

/** Current display height */
extern uint8_t s_displayHeight;

/** Current display width */
extern uint8_t s_displayWidth;

/** Current selected lcd display type */
extern uint8_t g_lcd_type;

#endif /* _LCD_COMMON_H_ */
