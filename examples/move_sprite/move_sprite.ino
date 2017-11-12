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

/* 
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes).
 */
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

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

/* Declare variable that represents our sprite */
SPRITE sprite;
int speedX = 1;
int speedY = 1;

void setup()
{
    /* Do not init Wire library for Attiny controllers */
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    /* Create sprite at 0,0 position. The function initializes sprite structure. */
    sprite = ssd1306_createSprite( 0, 0, spriteWidth, heartImage );
    /* Draw sprite on the display */
    sprite.draw();
}


void loop()
{
    /* Move sprite every 40 milliseconds */
    delay(40);
    sprite.x += speedX;
    sprite.y += speedY;
    /* If right boundary is reached, reverse X direction */
    if (sprite.x == (128 - spriteWidth)) speedX = -speedX;
    /* If left boundary is reached, reverse X direction */ 
    if (sprite.x == 0) speedX = -speedX;
    /* Sprite height is always 8 pixels. Reverse Y direction if bottom boundary is reached. */
    if (sprite.y == (64 - 8)) speedY = -speedY;
    /* If top boundary is reached, reverse Y direction */
    if (sprite.y == 0) speedY = -speedY;
    /* Erase sprite on old place. The library knows old position of the sprite. */
    sprite.eraseTrace();
    /* Draw sprite on new place */
    sprite.draw();
}




