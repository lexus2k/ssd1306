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
 *   Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(DIN), D13(CLK)
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *   ESP8266: connect LCD to D1(D/C), D2(CS), RX(RES), D7(DIN), D5(CLK)
 */

#include "ssd1306.h"
#include "ssd1331_api.h"
#include "nano_canvas.h"

/* Do not include SPI.h for Attiny controllers */
#ifdef SSD1306_SPI_SUPPORTED
    #include <SPI.h>
#endif

uint8_t buffer[32*32];

NanoCanvas8 canvas(32,32,buffer);
int x = 72;
int textx = 10;

void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1331_96x64_spi_init(3, 4, 5);
    ssd1306_fillScreen( 0x00 );
    ssd1331_setMode(0);
    canvas.setTextMode(TEXT_MODE_TRANSPARENT);
}

void drawAll()
{
    canvas.clear();
    canvas.drawRect(15,12,x,55,RGB_COLOR8(255,255,0));
    canvas.fillRect(16,13,x-1,54,RGB_COLOR8(0,255,255));
    canvas.putPixel(5,5,RGB_COLOR8(255,255,255));
    canvas.putPixel(10,10,RGB_COLOR8(255,0,0));
    canvas.putPixel(20,15,RGB_COLOR8(0,255,0));
    canvas.putPixel(30,20,RGB_COLOR8(0,0,255));
    canvas.setColor(RGB_COLOR8(255,0,0));
    canvas.printFixed(textx, 30, "This is example of text output", STYLE_NORMAL);
}

void refreshDisplay()
{
    canvas.setOffset(0,0); drawAll(); canvas.blt();
    canvas.setOffset(32,0); drawAll(); canvas.blt();
    canvas.setOffset(64,0); drawAll(); canvas.blt();
    canvas.setOffset(0,32); drawAll(); canvas.blt();
    canvas.setOffset(32,32); drawAll(); canvas.blt();
    canvas.setOffset(64,32); drawAll(); canvas.blt();
}


void loop()
{
    refreshDisplay();
    delay(20);
    textx++; if (textx ==96) textx = -192;
}