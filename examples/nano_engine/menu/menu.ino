/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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

NanoEngine1 engine;
NanoTextMenuItem<NanoEngine1> item1("First menu item");
NanoTestMenuItem<NanoEngine1> item2;
NanoTextMenuItem<NanoEngine1> item3("Demo");
NanoTestMenuItem<NanoEngine1> item4;
NanoFixedWidthMenu<NanoEngine1> menu( {0,0}, {128,64} );

uint16_t lastMillis;

void setup()
{
    ssd1306_128x64_i2c_init();
//    ssd1331_96x64_spi_init(3,4,5);
//    ssd1351_128x128_spi_init(3,4,5);
//    il9163_128x128_spi_init(3,4,5);
    ssd1306_setFixedFont(ssd1306xled_font6x8);

    engine.setFrameRate( 30 );
    engine.begin();

    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);

    menu.add( item1 );
    menu.add( item2 );
    menu.add( item3 );
    menu.add( item4 );
    engine.insert( menu );

    engine.refresh();
    lastMillis = millis();
}

void loop()
{
    if (static_cast<uint16_t>(millis() - lastMillis) > 2000 )
    {
        lastMillis = millis();
        menu.down();
    }
    if (!engine.nextFrame()) return;
    engine.update();
    engine.display();
}



