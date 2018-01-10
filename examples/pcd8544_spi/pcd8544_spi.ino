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
/**
 *
 *   Atmega328 PINS & Nokia 5110:
 *      D3 to RST pin of LCD 8544
 *      D4 to CES pin of LCD 8544
 *      D5 to DC pin of LCD 8544
 *      BL pin of LCD to VCC (if you need backlight)
 *      SCK(D13) to CLK pin of LCD 8544
 *      MOSI(D11) to DIN pin of LCD 8544
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
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
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
    pcd8544_84x48_spi_init(3, 4, 5);
    ssd1306_fillScreen(0x0);
    /* Set range of the SpritePool field on the screen in blocks. *
     * Use whole LCD display                                      *
     * 0,0   means left-top block of lcd display.                 *
     *                         that is 0*8=0 - pixel              *
     *                                 0*8=0 - pixel              *
     * 9,5   means right-bottom block of lcd:                     *
     *                         that is 9*8+7=79th pixel        *
     *                                 5*8+7=47th pixel          */
    s_pool.setRect( (SSD1306_RECT) { 0, 0, 9, 5 } );
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
    /* Move sprite every 60 milliseconds */
    delay(60);
    sprite.x += speedX;
    sprite.y += speedY;
    /* If right boundary is reached, reverse X direction */
    if (sprite.x == (84 - spriteWidth)) speedX = -speedX;
    /* If left boundary is reached, reverse X direction */ 
    if (sprite.x == 0) speedX = -speedX;
    /* Sprite height is always 8 pixels. Reverse Y direction if bottom boundary is reached. */
    if (sprite.y == (48 - 8)) speedY = -speedY;
    /* If top boundary is reached, reverse Y direction */
    if (sprite.y == 0) speedY = -speedY;

    /* Redraw only those areas, affected by sprites */
    s_pool.drawSprites();
}


