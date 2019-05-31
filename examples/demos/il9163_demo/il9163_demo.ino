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

/* !!! THIS DEMO RUNS in FULL COLOR MODE */

#include "ssd1306.h"
#include "nano_engine.h"
#include "sova.h"

/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes).
 */
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

const PROGMEM uint8_t heartImage8[ 8 * 8 ] =
{
    0x00, 0xE0, 0xE0, 0x00, 0x00, 0xE5, 0xE5, 0x00,
    0xE0, 0xC0, 0xE0, 0xE0, 0xE0, 0xEC, 0xEC, 0xE5,
    0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE5, 0xEC, 0xE5,
    0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE5, 0xE0,
    0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0x00,
    0x00, 0x00, 0x80, 0xE0, 0xE0, 0xE0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xE0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

SAppMenu menu;

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "nano canvas",
    "draw lines",
};

static void bitmapDemo()
{
    ssd1306_setColor(RGB_COLOR8(64,64,255));
    ssd1306_drawMonoBitmap8(0, 0, 128, 64, Sova);
    ssd1306_drawBitmap8(0, 0, 8, 8, heartImage8);
    ssd1306_setColor(RGB_COLOR8(255,64,64));
    ssd1306_drawMonoBitmap8(0, 16, 8, 8, heartImage);
    delay(3000);
}

/* Sprites are not implemented for color modes.
 * But there is NanoEngine support
 * To make example clear, we use lambda as function pointer. Since lambda can be
 * passed to function only if it doesn't capture, all variables should be global.
 * Refer to C++ documentation.
 */
NanoPoint sprite;
NanoEngine8 engine;
static void spriteDemo()
{
    // We not need to clear screen, engine will do it for us
    engine.begin();
    // Force engine to refresh the screen
    engine.refresh();
    // Set function to draw our sprite
    engine.drawCallback( []()->bool {
        engine.canvas.clear();
        engine.canvas.setColor( RGB_COLOR8(255, 32, 32) );
        engine.canvas.drawBitmap1( sprite.x, sprite.y, 8, 8, heartImage );
        return true;
    } );
    sprite.x = 0;
    sprite.y = 0;
    for (int i=0; i<250; i++)
    {
        delay(15);
        // Tell the engine to refresh screen at old sprite position
        engine.refresh( sprite.x, sprite.y, sprite.x + 8 - 1, sprite.y + 8 - 1 );
        sprite.x++;
        if (sprite.x >= ssd1306_displayWidth())
        {
            sprite.x = 0;
        }
        sprite.y++;
        if (sprite.y >= ssd1306_displayHeight())
        {
            sprite.y = 0;
        }
        // Tell the engine to refresh screen at new sprite position
        engine.refresh( sprite.x, sprite.y, sprite.x + 8 - 1, sprite.y + 8 - 1 );
        // Do refresh required parts of screen
        engine.display();
    }
}

static void textDemo()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed8(0,  8, "Normal text", STYLE_NORMAL);
    ssd1306_setColor(RGB_COLOR8(0,255,0));
    ssd1306_printFixed8(0, 16, "bold text?", STYLE_BOLD);
    ssd1306_setColor(RGB_COLOR8(0,255,255));
    ssd1306_printFixed8(0, 24, "Italic text?", STYLE_ITALIC);
    ssd1306_negativeMode();
    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(0, 32, "Inverted bold?", STYLE_BOLD);
    ssd1306_positiveMode();
    delay(3000);
}

static void canvasDemo()
{
    uint8_t buffer[64*16/8];
    NanoCanvas1_8 canvas(64,16, buffer);
    ssd1306_setColor(RGB_COLOR8(0,255,0));
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    canvas.clear();
    canvas.fillRect(10, 3, 80, 5);
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    delay(500);
    canvas.fillRect(50, 1, 60, 15);
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    delay(1500);
    canvas.printFixed(20, 1, " DEMO ", STYLE_BOLD );
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    delay(3000);
}

static void drawLinesDemo()
{
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    for (uint8_t y = 0; y < ssd1306_displayHeight(); y += 8)
    {
        ssd1306_drawLine8(0,0, ssd1306_displayWidth() -1, y);
    }
    ssd1306_setColor(RGB_COLOR8(0,255,0));
    for (uint8_t x = ssd1306_displayWidth() - 1; x > 7; x -= 8)
    {
        ssd1306_drawLine8(0,0, x, ssd1306_displayHeight() - 1);
    }
    delay(3000);
}

void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    il9163_128x128_spi_init(3, 4, 5);
//    il9163_128x128_spi_init(3, -1, 4); // Use this line for ATTINY
//    il9163_128x128_spi_init(22, 5, 21); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

    // RGB functions do not work in default SSD1306 compatible mode
    ssd1306_setMode( LCD_MODE_NORMAL );
    ssd1306_clearScreen8( );
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    ssd1306_showMenu8( &menu );
}

uint8_t rotation = 0;

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

        case 4:
            drawLinesDemo();
            break;

        default:
            break;
    }
    if ((menu.count - 1) == ssd1306_menuSelection(&menu))
    {
         il9163_setRotation((++rotation) & 0x03);
    }
    ssd1306_clearScreen8( );
    ssd1306_setColor(RGB_COLOR16(255,255,255));
    ssd1306_showMenu8(&menu);
    delay(500);
    ssd1306_menuDown(&menu);
    ssd1306_updateMenu8(&menu);
}
