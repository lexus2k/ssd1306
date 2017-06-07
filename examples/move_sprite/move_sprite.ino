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

/* Do not include wire.h for Attiny controllers */
#ifndef SSD1306_EMBEDDED_I2C
    #include <Wire.h>
#endif


const PROGMEM uint8_t heartImage[8] =
{
    B00001110,
    B00011111,
    B00111111,
    B01111110,
    B01111110,
    B00111101,
    B00011001,
    B00001110
};

const int spriteWidth = 8;

SPRITE sprite;
int speedX = 1;
int speedY = 1;

void setup()
{
    /* Do not init Wire library for Attiny controllers */
#ifndef SSD1306_EMBEDDED_I2C
    Wire.begin();
#endif
    ssd1306_init();
    ssd1306_fillScreen(0x00);
    sprite = ssd1306_createSprite( 0, 0, spriteWidth, heartImage );
    ssd1306_drawSprite( &sprite );
}


void loop()
{
    delay(40);
    sprite.x += speedX;
    sprite.y += speedY;
    if (sprite.x == (128 - spriteWidth)) speedX = -speedX;
    if (sprite.x == 0) speedX = -speedX;
    /* Sprite height is always 8 pixels */
    if (sprite.y == (64 - 8)) speedY = -speedY;
    if (sprite.y == 0) speedY = -speedY;
    ssd1306_eraseTrace( &sprite );
    ssd1306_drawSprite( &sprite );
}




