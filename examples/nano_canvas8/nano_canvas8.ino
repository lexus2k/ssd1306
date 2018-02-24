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
#include "sova.h"

#define TILE_SIZE   16

uint8_t buffer[TILE_SIZE*TILE_SIZE];

NanoCanvas8 canvas(TILE_SIZE,TILE_SIZE,buffer);

int x = 72;
int b_x = -128;
int b_y = -128;
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
    canvas.fillRect(16,13,x-1,54,RGB_COLOR8(64,64,64));
    canvas.setColor(RGB_COLOR8(0,255,255));
    canvas.drawBitmap1(b_x, b_y, 128, 64, Sova);
    canvas.putPixel(5,5,RGB_COLOR8(255,255,255));
    canvas.putPixel(10,10,RGB_COLOR8(255,0,0));
    canvas.putPixel(20,15,RGB_COLOR8(0,255,0));
    canvas.putPixel(30,20,RGB_COLOR8(0,0,255));
    canvas.setColor(RGB_COLOR8(255,0,0));
    canvas.printFixed(textx, 30, "This is example of text output", STYLE_NORMAL);
}

void refreshDisplay()
{
    for (int y=0; y<64; y = y + TILE_SIZE)
    {
        for(int x=0; x<96; x = x + TILE_SIZE)
        {
            canvas.setOffset(x,y);
            drawAll();
            canvas.blt();
        }
    }
}


void loop()
{
    refreshDisplay();
    delay(30);
    textx++; if (textx ==96) textx = -192;
    b_x++;
    b_y++;
}