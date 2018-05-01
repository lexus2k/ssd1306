/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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
/**
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Atmega328 PINS: connect LCD to A4/A5
 */

#include "ssd1306.h"

const uint8_t g_customFont_5x16[] PROGMEM =
{
    0x00,  // 0x00 means fixed font type - the only supported by the library
    0x05,  // 0x05 = 5 - font width in pixels
    0x10,  // 0x10 = 16 - font height in pixels
    0x30,  // 0x30 = 48 - first ascii character number in the font ('0' = ascii code 48)
    // '0'
    0b00000000,  // upper 8 pixels of character
    0b11111100,
    0b00000011,
    0b00000011,
    0b11111100,

    0b00000000,  // lower 8 pixels of character
    0b00011111,
    0b01100000,
    0b01100000,
    0b00011111,
    // '1'
    0b00000000,  // upper 8 pixels of character
    0b00001100,
    0b11111111,
    0b00000000,
    0b00000000,

    0b00000000,  // lower 8 pixels of character
    0b01100000,
    0b01111111,
    0b01100000,
    0b00000000,

    0x00, // End of font
};

void setup()
{
    /* Replace the line below with ssd1306_128x32_i2c_init() if you need to use 128x32 display */
    ssd1306_128x64_i2c_init();
//    ssd1306_128x64_spi_init(3, 4, 5);
    ssd1306_fillScreen(0x00);
    ssd1306_setFixedFont( g_customFont_5x16 );
    ssd1306_printFixed (0,  0,  "01100011", STYLE_NORMAL );
    ssd1306_printFixedN (0, 16, "1001", STYLE_NORMAL, FONT_SIZE_2X);
}


void loop()
{
}




