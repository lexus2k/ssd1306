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
#include "ssd1306_i2c.h"
#include "nano_gfx.h"

/* Do not include wire.h for Attiny controllers */
#ifndef SSD1306_EMBEDDED_I2C
    #include <Wire.h>
#endif

/* 
 * Mountain image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes). 16x8
 */
const PROGMEM uint8_t mountainImage[16] =
{
    B10000000,
    B10000000,
    B01000000,
    B01000000,
    B10100000,
    B01010000,
    B01001000,
    B10100110,
    B01001001,
    B00100110,
    B10101000,
    B00010000,
    B00100000,
    B11000000,
    B01000000,
    B10000000
};

/* 
 * Ground image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes). 16x8
 */
const PROGMEM uint8_t groundImage[16] =
{
    B11111110,
    B01111111,
    B11111111,
    B11111110,
    B11111110,
    B11111100,
    B11111111,
    B11111111,
    B01111110,
    B11111110,
    B01111110,
    B11111110,
    B11111111,
    B11111111,
    B11111111,
    B10101010
};

/* 
 * Lower part of ground image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes). 16x8
 */
const PROGMEM uint8_t bottomImage[16] =
{
    B10001111,
    B01010101,
    B00111111,
    B01000101,
    B10101111,
    B01010111,
    B10101101,
    B01010111,
    B10001011,
    B01000101,
    B01010111,
    B00101111,
    B01000101,
    B00101011,
    B01000101,
    B10101010
};

/* Lets start from the beginning */
uint16_t screenPos = 0;

/* Field defines background, which contains 2 mountains */
uint8_t field[1][8] =
{
    { 0, 0, 0, 0, 1, 0, 1, 0 }
};

void drawScreen()
{
    /* Calculate position offset for each layer to start draw with. */
    /* Background with mountains moves 4 times slower than upper ground */
    uint8_t topOffset    = 0 - ((screenPos >> 2) & 0x0F);
    /* Upper ground moves normally */
    uint8_t groundOffset = 0 - (screenPos & 0x0F);
    /* Upper ground moves normally */
    uint8_t bottomOffset = 0 - ((screenPos << 1) & 0x0F);
    for( uint8_t x = 0; x < 8; x++ )
    {
        /* Calculate block offset from the first block x<<4 = x*16 */
        uint8_t blockOffset = (x<<4);
        /* Mountains are moved 4 times slower than ground, so real block
           index will be (screenPos/16)/4 = (screenPos>>4)>>2 = screenPos >> 6 */
        uint8_t mountain = field[0][((screenPos >> 6) + x) & 0x07];
        if ( mountain )
        {
            ssd1306_drawBitmap(blockOffset + topOffset, 4, 16, 8, mountainImage);
        }
        else
        {
            ssd1306_clearBlock(blockOffset + topOffset, 4, 16, 8);
        }
        ssd1306_drawBitmap(blockOffset + groundOffset, 5, 16, 8, groundImage);
        ssd1306_drawBitmap(blockOffset + bottomOffset, 6, 16, 8, bottomImage);
    }
}


void setup()
{
    /* Do not init Wire library for Attiny controllers */
#ifndef SSD1306_EMBEDDED_I2C
    Wire.begin();
#endif
    /* Initialize and clear display */
    ssd1306_init();
    ssd1306_fillScreen(0x00);
}


void loop()
{
    delay(20);
    drawScreen();
    /* Move screen forward every 20 milliseconds */
    screenPos += 1;
}
