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
 *   Atmega328 PINS: connect LCD to A4/A5
 *   This sketch is just for test purposes. For those, who wants to compare libraries
 */

#include "Wire.h"

/* Uncomment if you want to run test with Adafruit library */
//#define ADAFRUIT_TEST

#ifdef ADAFRUIT_TEST
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#else
#include "ssd1306.h"
#include "nano_gfx.h"
#include "i2c/ssd1306_i2c_wire.h"
#endif


#ifdef ADAFRUIT_TEST
    Adafruit_SSD1306 display(4);
#else
    uint8_t buf[1024];
    NanoCanvas canvas(128, 64, buf);
#endif

/** Returns delay */
unsigned long runTest()
{
    unsigned long ts = micros();
#ifdef ADAFRUIT_TEST
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("TEST MESSAGE");
    display.setCursor(0,20);
    display.print("I2C_CLOCK");
    display.setCursor(0,40);
    display.print("ADC VALUE");
    display.display();
#else
    canvas.clear();
    canvas.charF6x8(0,0, "TEST MESSAGE");
    canvas.charF6x8(0,20, "I2C_CLOCK");
    canvas.charF6x8(0,40, "ADC VALUE");
    canvas.blt(0,0);
#endif
    return (micros() - ts);
}

void printResult(unsigned long ms)
{
#ifdef ADAFRUIT_TEST
    display.setCursor(32, 56);
    display.print(ms);
    display.setCursor(90, 56);
    display.print("MS");
    display.display();
#else
    char buf[16];
    utoa(ms,buf,10);
    ssd1306_printFixedN(32, 7, buf, STYLE_BOLD, 0);
    ssd1306_printFixedN(90, 7, "MS", STYLE_BOLD, 0);
#endif
}

void setup()
{
    /* Replace the line below with ssd1306_128x32_i2c_init() if you need to use 128x32 display */
    Wire.begin();
    /* Max supported frequency by ssd1306 controller. Please refer to datasheet */
    Wire.setClock( 400000 );

#ifdef ADAFRUIT_TEST
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.display();
#else    
    ssd1306_i2cInit_Wire(0x3C);
    ssd1306_128x64_init();
    ssd1306_clearScreen();
#endif

}

void loop()
{
    if ((millis() & 0x3FF) == 0)
    {
        unsigned long result = runTest();
        printResult(result);
    }
}
