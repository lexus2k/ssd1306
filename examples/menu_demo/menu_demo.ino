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
/*
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0) - BUTTONS module
 *
 *   Atmega328 PINS: connect LCD to A4/A5,
 *   Z-keypad ADC module on A0 pin.
 */

#include "ssd1306.h"
#include "buttons.h"

#define BUTTON_PIN     A0

/* Define menu items of the menu box */
const char *menuItems[] =
{
    "menu item 1",
    "menu item 2",
    "menu item 3",
    "menu item 4",
    "menu item 5",
    "menu item 6",
    "menu item 7",
    "menu item 8",
};

/* This variable will hold menu state, processed by SSD1306 API functions */
SAppMenu menu;


void setup()
{
    /* Do not init Wire library for Attiny controllers */
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );
    /* Initialize main menu state */
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    /* show menu on the display */
    ssd1306_showMenu( &menu );
}


void loop()
{
    switch (getPressedButton(BUTTON_PIN))
    {
        case BUTTON_UP:
            /* move menu cursor up and refresh menu on the display */
            ssd1306_menuUp( &menu );
            ssd1306_updateMenu( &menu );
            break;
        case BUTTON_DOWN:
            /* move menu cursor down and refresh menu on the display */
            ssd1306_menuDown( &menu );
            ssd1306_updateMenu( &menu );
            break;
        case BUTTON_SELECT:
            /* You always can request current position of menu cursor, by calling ssd1306_menuSelection() */
            break;
        default:
            break;
    }
}




