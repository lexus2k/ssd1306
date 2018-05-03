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
#include "nano_engine.h"

/*
 * Each pixel in SSD1306 display takes 1 bit of the memory. So, full resolution
 * of 128x64 LCD display will require 128*64/8 = 1024 bytes of SRAM for the buffer.
 * To let this example to run on Attiny devices (they have 256/512 byte SRAM), we
 * will use small canvas buffer: 32x32 (requires 128 bytes of SRAM), so the example
 * would run even on Attiny45.
 */
const int canvasWidth = 32; // Width must be power of 2, i.e. 16, 32, 64, 128...
const int canvasHeight = 32; // Height must be divided on 8, i.e. 8, 16, 24, 32...
uint8_t canvasData[canvasWidth*(canvasHeight/8)];
/* Create canvas object */
NanoCanvas1 canvas(canvasWidth, canvasHeight, canvasData);

void setup()
{
    /* Initialize and clear display */
    ssd1306_128x64_i2c_init();
//    ssd1306_128x64_i2c_init();
//    pcd8544_84x48_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS
//    ssd1331_96x64_spi_init(3, 4, 5);
//    ssd1351_128x128_spi_init(3, 4, 5);
//    il9163_128x128_spi_init(3, 4, 5);
//    st7735_128x160_spi_init(3, 4, 5);

    ssd1306_clearScreen();
    /* Set font to use with canvas in console mode */
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    /* Make text to wrap to new line automatically at the end of buffer */
    canvas.setMode( CANVAS_TEXT_WRAP_LOCAL );
}


void loop()
{
    /* Here use any methods, provided by Arduino Print class */
    canvas.print( "Start: " );
    canvas.println( "<LF>" );
    delay(500);
    canvas.blt();
}




