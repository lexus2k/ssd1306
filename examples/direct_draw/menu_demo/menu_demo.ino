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




