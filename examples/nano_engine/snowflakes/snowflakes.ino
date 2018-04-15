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
 *   Attiny85  I2C PINS: see picture above
 *   Digispark I2C PINS: D0/D2
 *   Atmega328 I2C PINS: connect LCD to A4/A5
 *   ESP8266   I2C PINS: GPIO4/GPIO5
 *   ESP32     I2C PINS: 21/22
 */

#include "ssd1306.h"
#include "nano_engine.h"

typedef struct
{
    const uint8_t * bitmap;
    int16_t x;
    int16_t y;
    int8_t speedX;
    int8_t speedY;
    uint8_t timer;
    bool    falling;
} SnowFlake;

/* 
 * Define snowflake images directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to right (bytes).
 */
const PROGMEM uint8_t snowFlakeImage[8][8] =
{
    {
        0B00111000,
        0B01010100,
        0B10010010,
        0B11111110,
        0B10010010,
        0B01010100,
        0B00111000,
        0B00000000
    },
    {
        0B00010000,
        0B01010100,
        0B00111000,
        0B11101110,
        0B00111000,
        0B01010100,
        0B00010000,
        0B00000000
    },
    {
        0B00111000,
        0B00010000,
        0B10111010,
        0B11101110,
        0B10111010,
        0B00010000,
        0B00111000,
        0B00000000
    },
    {
        0B00011000,
        0B01011010,
        0B00100100,
        0B11011011,
        0B11011011,
        0B00100100,
        0B01011010,
        0B00011000
    },
    {
        0B00010000,
        0B00111000,
        0B01010100,
        0B11101110,
        0B01010100,
        0B00111000,
        0B00010000,
        0B00000000
    },
    {
        0B10000010,
        0B00101000,
        0B01101100,
        0B00010000,
        0B01101100,
        0B00101000,
        0B10000010,
        0B00000000
    },
    {
        0B01000100,
        0B10101010,
        0B01101100,
        0B00010000,
        0B01101100,
        0B10101010,
        0B01000100,
        0B00000000
    },
    {
        0B00101000,
        0B01010100,
        0B10111010,
        0B01101100,
        0B10111010,
        0B01010100,
        0B00101000,
        0B00000000
    },
};


NanoEngine1 engine;

static const uint8_t maxCount = 20;

/* These are our snow flakes */
SnowFlake snowFlakes[maxCount] = { {0} };

bool onDraw()
{
    engine.canvas.clear();
    for (uint8_t i=0; i<maxCount; i++)
    {
        if (snowFlakes[i].falling)
        {
            engine.canvas.drawBitmap1(snowFlakes[i].x>>3, snowFlakes[i].y>>3, 8, 8,snowFlakes[i].bitmap);
        }
    }
    return true;
}

void setup()
{
    /* Do not init Wire library for Attiny controllers */
    ssd1306_128x64_i2c_init();
//    ssd1331_96x64_spi_init(3,4,5);
//    ssd1351_128x128_spi_init(3,4,5);

    engine.setFrameRate( 30 );
    engine.begin();
    engine.drawCallback( onDraw );

    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);
    engine.refresh();
}

void addSnowFlake()
{
    for (uint8_t i=0; i<maxCount; i++)
    {
        if (!snowFlakes[i].falling)
        {
            /* We found empty slot, use it for new snowflake */
            /* Create new sprite with new snowflake image. Just put it for now to fixed place */
            snowFlakes[i].bitmap = &snowFlakeImage[random(8)][0];
            /* Select horizontal position */
            snowFlakes[i].x = random(ssd1306_displayWidth() * 8);
            /* Select y position. We use number multiple of 8 to make snowflake movements more smooth. *
             * Moving snowflake by 1 point actually means movement by 1/8 of pixel on the display.     */
            snowFlakes[i].y = -8 * 8;
            /* Use some random X speed */
            snowFlakes[i].speedX = random(-16, 16);
            /* Use random vertical speed */
            snowFlakes[i].speedY = random(4, 12);
            snowFlakes[i].falling = true;
            /* After countdown timer ticks to 0, change X direction */
            snowFlakes[i].timer = random(24, 48);
            /* And register new snowflake in spritepool */
            engine.refresh( snowFlakes[i].x/8, snowFlakes[i].y/8, (snowFlakes[i].x/8) + 7, (snowFlakes[i].y/8) + 7);
            break;
        }
    }
}


void moveSnowFlakes()
{
    for (uint8_t i=0; i<maxCount; i++)
    {
        if (snowFlakes[i].falling)
        {
            engine.refresh( snowFlakes[i].x/8, snowFlakes[i].y/8, (snowFlakes[i].x/8) + 7, (snowFlakes[i].y/8) + 7);
            snowFlakes[i].x += snowFlakes[i].speedX;
            snowFlakes[i].y += snowFlakes[i].speedY;
            snowFlakes[i].timer--;
            if (0 == snowFlakes[i].timer)
            {
                /* Change movement direction */
                snowFlakes[i].speedX = random(-16, 16);
                snowFlakes[i].timer = random(24, 48);
            }
            engine.refresh( snowFlakes[i].x/8, snowFlakes[i].y/8, (snowFlakes[i].x/8) + 7, (snowFlakes[i].y/8) + 7);
            if (snowFlakes[i].y >= ssd1306_displayHeight() * 8)
            {
                snowFlakes[i].falling = false;
            }
        }
    }
}


static uint8_t globalTimer=3;

void loop()
{
    if (!engine.nextFrame()) return;
    if (0 == (--globalTimer))
    {
        /* Try to add new snowflake every ~ 90ms */
        globalTimer = 3;
        addSnowFlake();
    }
    moveSnowFlakes();
    engine.display();
}



