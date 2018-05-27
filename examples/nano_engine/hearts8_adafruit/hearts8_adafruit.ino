/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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

// Define this before including library header, this will give Adafruit GFX support
// !!! Don't forget to install AdafruitGFX library to your Arduino IDE !!!
#define CONFIG_ADAFRUIT_GFX_ENABLE

#include "ssd1306.h"
#include "nano_engine.h"

// No need to include this for Arduino sketch
#ifndef ARDUINO
extern "C" void __cxa_pure_virtual() { while (1); }
#endif

/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is define from left to rigth (bits), from top to
 * bottom (bytes). Unlike NanoCanvas1 of ssd1306 library, Adafruit
 * GFX implementation uses different aproach for images, stored in Flash.
 */
const PROGMEM uint8_t heartImage[8] =
{
    0B01100110,
    0B11111001,
    0B11111101,
    0B11111111,
    0B01111110,
    0B00111100,
    0B00011000,
    0B00000000
};

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

/* We use 8-pixels height sprites */
const int spriteHeight = 8;

/* Lets show 4 hearts on the display */
const int spritesCount = 4;

/* Declare variable that represents our 4 objects */
struct
{
    NanoPoint pos;
    NanoPoint speed;
} objects[ spritesCount ];

/* Create engine object */
NanoEngine<ADATILE_8x8_RGB8> engine;

/* Array of colors, used for the heart sprites */
static uint8_t s_colors[spritesCount] =
{
     RGB_COLOR8(255,0,0),
     RGB_COLOR8(0,255,0),
     RGB_COLOR8(0,0,255),
     RGB_COLOR8(255,255,0),
};

/*
 * This function is called by the engine every time, it needs to refresh
 * some part of the display. Just draw the content as usual. NanoCanvas
 * will do correct clipping for you.
 */
bool drawHearts()
{
    /* Clear canvas surface */
    engine.canvas.fillScreen(0);
    /* Draw line */
    engine.canvas.drawLine( 0, 0, ssd1306_displayWidth()*2 - 1, ssd1306_displayHeight()-1, RGB_COLOR8(128,128,128));
    /* Draw rectangle around our canvas. It will show the range of the canvas on the display */
    engine.canvas.drawRect(0, 0, ssd1306_displayWidth(), ssd1306_displayHeight(), RGB_COLOR8(0,255,255));
    /* Draw all 4 sprites on the canvas */
    for (uint8_t i = 0; i < spritesCount; i++)
    {
        engine.canvas.drawBitmap( objects[i].pos.x, objects[i].pos.y, heartImage, 8, 8, s_colors[i] );
    }
    /* Now, return true to draw canvas on the display. *
     * If you return false, the part of display will not be refreshed */
    return true;
}

void setup()
{
    /* Initialize and clear display: 3 RST, 4 CES, 5 DS */
    il9163_128x128_spi_init(3, 4, 5);
//    ssd1331_96x64_spi_init(3, 4, 5);
//    ssd1351_128x128_spi_init(3, 4, 5);
//    st7735_128x160_spi_init(3, 4, 5);
//    -- ssd1306_128x64_i2c_init();  // RGB canvas does not support monochrome displays
//    -- pcd8544_84x48_spi_init(3, 4, 5);

    /* Start the engine. It will switch display to required mode */
    engine.begin();

    /* Set frame refresh rate: 45 is ok for the eye */
    engine.setFrameRate(45);

    /* Make the engine to redraw whole display content when the board is powered on. *
     * refresh() function do not change display content, but says notifies engine that *
     * it needs to refresh() whole screen on next frame. */
    engine.refresh();

    /* Create 4 "hearts", and place them at different positions and give different movement direction */
    for(uint8_t i = 0; i < spritesCount; i++)
    {
        objects[i].speed = { .x = (i & 2) ? -1:  1, .y = (i & 1) ? -1:  1 };
        objects[i].pos = { .x = i*16, .y = i*8 + 2 };
    }

    /* Here we set draw callback, so the engine will call it every time it needs *
     * physically update display content */
    engine.drawCallback( drawHearts );
}

void loop()
{
    /* If it is not time to draw next frame just exit and do nothing */
    if (!engine.nextFrame()) return;

    /* Recalculate position and movement direction of all 4 "hearts" */
    for (uint8_t i = 0; i < spritesCount; i++)
    {
        /* We need to point the old position of the heart sprite */
        engine.refresh( objects[i].pos.x, objects[i].pos.y,
                        objects[i].pos.x + spriteWidth - 1,
                        objects[i].pos.y + spriteHeight - 1 );
        objects[i].pos += objects[i].speed;
        /* If left or right boundary is reached, reverse X direction */
        if ((objects[i].pos.x == (ssd1306_displayWidth() - 8)) || (objects[i].pos.x == 0))
            objects[i].speed.x = -objects[i].speed.x;
        /* Sprite height is always 8 pixels. Reverse Y direction if bottom or top boundary is reached. */
        if ((objects[i].pos.y == (ssd1306_displayHeight() - 8)) || (objects[i].pos.y == 0))
            objects[i].speed.y = -objects[i].speed.y;
        /* Now provide the new position of the heart sprite to engine */
        engine.refresh( objects[i].pos.x, objects[i].pos.y,
                        objects[i].pos.x + spriteWidth - 1,
                        objects[i].pos.y + spriteHeight - 1 );
    }
    /* Now do updates on the display */
    engine.display();
}
