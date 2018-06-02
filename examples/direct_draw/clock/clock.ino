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

#include "ssd1306.h"

uint32_t lastMillis;
uint8_t  hours = 10;
uint8_t  minutes = 35;
uint8_t  seconds = 0;

void printSeconds()
{
    if (seconds & 1)
    {
        ssd1306_printFixed(54,  8, ":", STYLE_NORMAL);
    }
    else
    {
        ssd1306_printFixed(54,  8, " ", STYLE_NORMAL);
    }
}

void printMinutes()
{
    char minutesStr[3] = "00";
    minutesStr[0] = '0' + minutes / 10;
    minutesStr[1] = '0' + minutes % 10;
    ssd1306_printFixed(78,  8, minutesStr, STYLE_NORMAL);
}

void printHours()
{
    char hoursStr[3] = "00";
    hoursStr[0] = '0' + hours / 10;
    hoursStr[1] = '0' + hours % 10;
    ssd1306_printFixed(6,  8, hoursStr, STYLE_NORMAL);
}

void setup() {
    /* Replace the line below with ssd1306_128x32_i2c_init() if you need to use 128x32 display */
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    ssd1306_setFixedFont(comic_sans_font24x32_123);
    lastMillis = millis();
    printHours();
    printMinutes();
}

void loop()
{
    if ((uint32_t)(millis() - lastMillis) >= 1000)
    {
        lastMillis += 1000;
        if (++seconds > 59)
        {
            seconds = 0;
            if (++minutes > 59)
            {
                minutes = 0;
                if (++hours > 23)
                {
                    hours = 0;
                }
                printHours();
            }
            printMinutes();
        }
        printSeconds();
    }
}




