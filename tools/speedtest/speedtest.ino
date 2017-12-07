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
    ssd1306_charF6x8(32, 7, buf, STYLE_BOLD);
    ssd1306_charF6x8(90, 7, "MS", STYLE_BOLD);
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
