/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

DisplaySSD1331_96x64x16_SPI display(4, 5);

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

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */

SAppMenu menu;

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "nano canvas",
    "draw lines",
};

uint8_t rotation = 0;

static void bitmapDemo()
{
    display.setColor(RGB_COLOR16(64,64,255));
    if ( rotation & 1 )
        display.drawBitmap1(0, 0, 64, 64, Owl_64x64);
    else
        display.drawBitmap1(0, 0, 96, 64, Owl);
    display.drawBitmap8(0, 0, 8, 8, heartImage8);
    display.setColor(RGB_COLOR16(255,64,64));
    display.drawBitmap1(0, 16, 8, 8, heartImage);
    delay(3000);
}

/* Sprites are not implemented for color modes.
 * But there is NanoEngine support
 * To make example clear, we use lambda as function pointer. Since lambda can be
 * passed to function only if it doesn't capture, all variables should be global.
 * Refer to C++ documentation.
 */
NanoPoint sprite;
NanoEngine16 engine(display);
static void spriteDemo()
{
    // We not need to clear screen, engine will do it for us
    engine.begin();
    // Force engine to refresh the screen
    engine.refresh();
    // Set function to draw our sprite
    engine.drawCallback( []()->bool {
        engine.canvas.clear();
        engine.canvas.setColor( RGB_COLOR16(255, 32, 32) );
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
        if (sprite.x >= display.width())
        {
            sprite.x = 0;
        }
        sprite.y++;
        if (sprite.y >= display.height())
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
    display.clear();
    ssd1306_setFixedFont(digital_font5x7);
    display.setColor(RGB_COLOR16(0,64,255));
    display.printFixed(0,  0, "0123456789", STYLE_NORMAL);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    display.setColor(RGB_COLOR16(255,255,0));
    display.printFixed(0,  8, "Normal text", STYLE_NORMAL);
    display.setColor(RGB_COLOR16(0,255,0));
    display.printFixed(0, 16, "Bold text?", STYLE_BOLD);
    display.setColor(RGB_COLOR16(0,255,255));
    display.printFixed(0, 24, "Italic text?", STYLE_ITALIC);
    display.negativeMode();
    display.setColor(RGB_COLOR16(255,255,255));
    display.printFixed(0, 32, "Inverted bold?", STYLE_BOLD);
    display.positiveMode();
    delay(3000);
}

static void canvasDemo()
{
    NanoCanvas<64,16,1> canvas;
    display.setColor(RGB_COLOR16(0,255,0));
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    display.clear();
    canvas.clear();
    canvas.fillRect(10, 3, 80, 5);
    display.drawCanvas( (display.width()-64)/2, 1, canvas);
    delay(500);
    canvas.fillRect(50, 1, 60, 15);
    display.drawCanvas( (display.width()-64)/2, 1, canvas);
    delay(1500);
    canvas.printFixed(20, 1, " DEMO ", STYLE_BOLD );
    display.drawCanvas( (display.width()-64)/2, 1, canvas);
    delay(3000);
}

static void drawLinesDemo()
{
    /* SSD1331 controller has hardware acceleration, thus                *
     * use hw ssd1331_drawLine() instead of software ssd1331_drawLine8() */
    display.clear();
    for (lcduint_t y = 0; y < display.height(); y += 8)
    {
        display.drawLine(0,0, display.width() -1, y, RGB_COLOR16(0,255,0));
    }
    for (lcduint_t x = display.width() - 1; x > 7; x -= 8)
    {
        display.drawLine(0,0, x, display.height() - 1, RGB_COLOR16(0,0,255));
    }
    delay(3000);
}

void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    display.begin();
//    ssd1331_96x64_spi_init(3, 4, 5);  // Use this line for Atmega328p
//    ssd1331_96x64_spi_init(3, -1, 4); // Use this line for ATTINY
//    ssd1331_96x64_spi_init(24, 0, 23); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//    ssd1331_96x64_spi_init(22, 5, 21); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

    // RGB functions do not work in default SSD1306 compatible mode
    display.fill( 0x00 );
    display.setColor(RGB_COLOR16(255,255,255));
    display.createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    display.showMenu( &menu );
}

void loop()
{
    delay(1000);
    switch (display.menuSelection(&menu))
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
    if ((menu.count - 1) == display.menuSelection(&menu))
    {
         display.setRotation((++rotation) & 0x03);
    }
    display.fill( 0x0000 );
    display.setColor(RGB_COLOR16(255,255,255));
    display.showMenu(&menu);
    delay(500);
    display.menuDown(&menu);
    display.updateMenu(&menu);
}
