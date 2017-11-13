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
#include "sprite_pool.h"

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

/* Declare sprite pool object here                            *
 * Sprite pool is responsible for update LCD's areas, touched *
 * by sprites. By default, it clears area under the sprite -  *
 * see SpritePool::drawBlock() library method. But you may    *
 * override that method, and draw anything you need for the   *
 * block, defined by column and row.                          */
SpritePool s_pool;


/* Declare variable that represents our sprite */
SPRITE sprite;
int speedX = 1;
int speedY = 1;

void setup()
{
    /* Do not init Wire library for Attiny controllers */
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    /* Set range of the SpritePool field on the screen in blocks. *
     * Use whole LCD display                                      *
     * 0,0   means left-top block of lcd display.                 *
     *                         that is 0*8=0 - pixel              *
     *                                 0*8=0 - pixel              *
     * 15,7  means right-bottom block of lcd:                     *
     *                         that is 15*8+7=127-th pixel        *
     *                                 7*8+7=63-rd pixel          */
    s_pool.setRect( (SSD1306_RECT) { 0, 0, 15, 7 } );
    /* Create sprite at 32,8 position in pixels. The function initializes sprite structure. */
    sprite = ssd1306_createSprite( 32, 8, spriteWidth, heartImage );
    /* Add created sprite to the sprite pool.                     *
     * Sprite pool doesn't store sprites themselve, only pointers,*
     * so, do not use local variables for the sprites.            */
    s_pool.add( sprite );
    /* Redraw whole LCD-display */
    s_pool.refreshScreen();
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

    /* Redraw only those areas, affected by sprites */
    s_pool.drawSprites();
}




