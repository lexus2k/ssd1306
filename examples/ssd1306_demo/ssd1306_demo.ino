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
 *   Attiny85 PINS (i2c)
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Atmega328 PINS: connect LCD to A4/A5 (i2c)
 */

#include "ssd1306.h"
#include "sova.h"
#include "nano_gfx.h"

/* Do not include SPI.h for Attiny controllers */
#ifdef SSD1306_SPI_SUPPORTED
    #include <SPI.h>
#endif

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

SAppMenu menu;

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "canvas gfx",
    "sprite pool",
};

static void bitmapDemo()
{
    ssd1306_drawBitmap(0, 0, 128, 64, Sova);
    delay(1000);
    ssd1306_invertMode();
    delay(2000);
    ssd1306_normalMode();
}

static void spriteDemo()
{
    ssd1306_clearScreen();
    /* Declare variable that represents our sprite */
    SPRITE sprite;
    /* Create sprite at 0,0 position. The function initializes sprite structure. */
    sprite = ssd1306_createSprite( 0, 0, spriteWidth, heartImage );
    for (int i=0; i<300; i++)
    {
        delay(10);
        sprite.x = (sprite.x + 1) & 0x7F;
        sprite.y = (sprite.y + 1) & 0x3F;
        /* Erase sprite on old place. The library knows old position of the sprite. */
        sprite.eraseTrace();
        /* Draw sprite on new place */
        sprite.draw();
    }
}

static void textDemo()
{
    ssd1306_clearScreen();
    ssd1306_charF6x8(0, 1, "Normal text");
    ssd1306_charF6x8(0, 2, "Bold text", STYLE_BOLD);
    ssd1306_charF6x8(0, 3, "Italic text", STYLE_ITALIC);
    ssd1306_negativeMode();
    ssd1306_charF6x8(0, 4, "Inverted bold", STYLE_BOLD);
    ssd1306_positiveMode();
    delay(3000);
}

static void canvasDemo()
{
    uint8_t buffer[64*16/8];
    NanoCanvas canvas(64,16, buffer);
    ssd1306_clearScreen();
    canvas.clear();
    canvas.fillRect(10, 3, 80, 5, 0xFF);
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    delay(500);
    canvas.fillRect(50, 1, 60, 15, 0xFF);
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    delay(1500);
    canvas.charF6x8(20, 1, " DEMO " );
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    delay(3000);
}

void setup()
{
    /* Do not init Wire library for Attiny controllers */
    ssd1306_128x64_i2c_init();

    /* Uncomment lines below if you want to use SPI Nokia 5110 LCD */
/*
    pcd8544_84x48_spi_init(3, 4, 5);
*/

    ssd1306_fillScreen( 0x00 );
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    ssd1306_showMenu( &menu );
}

void loop()
{
    delay(1000);
    switch (ssd1306_menuSelection(&menu))
    {
        case 0:
            bitmapDemo();
            break;

        case 1:
            spriteDemo();
            break;

        case 2:
            textDemo();
            break;

        case 3:
            canvasDemo();
            break;
        default:
            break;
    }
    ssd1306_fillScreen( 0x00 );
    ssd1306_showMenu(&menu);
    delay(500);
    ssd1306_menuDown(&menu);
    ssd1306_updateMenu(&menu);
}




