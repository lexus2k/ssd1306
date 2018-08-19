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
 *   !!!!!!!!!!!!!!!!!!         RUNS ON ATMEGA328P ONLY   !!!!!!!!!!!!!!!!!!!!
 *   Nano/Atmega328 PINS:
 *     A0 - blue channel of D-Sub connector
 *     A1 - green channel of D-Sub connector
 *     A2 - red channel of D-Sub connector
 *     D9 - H-Sync of D-Sub connector
 *     D3 - V-Sync of D-Sub connector
 *
 *   Sketch allows to use all other PINs except A3-A6 pins.
 *   delay, millis functions do not work. Use ssd1306_vga_delay() instead.
 *
 *   Controlling VGA requires local frame buffer to be located in RAM. Enabling VGA needs 1450 bytes (almost all Atmega328p RAM).
 *   So, be careful with application stack.
 */
#include "ssd1306.h"
#include "nano_engine.h"
#include "sova.h"
#define CONFIG_VGA_96X40_ENABLE
#include "intf/vga/atmega328p/vga_isr.h"
#include "lcd/vga_monitor.h"
#include "intf/ssd1306_interface.h"

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

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

SAppMenu menu;

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "canvas gfx",
    "draw lines",
};

static void bitmapDemo()
{
    ssd1306_setColor(RGB_COLOR8(64,64,255));
    ssd1306_drawMonoBitmap8(0, 0, 96, 64, Sova);
    ssd1306_vga_delay(3000);
}

/* Sprites are not implemented for color modes.
 * But there is NanoEngine support
 * To make example clear, we use lambda as function pointer. Since lambda can be
 * passed to function only if it doesn't capture, all variables should be global.
 * Refer to C++ documentation.
 */
NanoPoint sprite;
NanoEngine1_8 engine;
static void spriteDemo()
{
    // We not need to clear screen, engine will do it for us
    engine.begin();
    // Force engine to refresh the screen
    engine.refresh();
    // Set function to draw our sprite
    engine.drawCallback( []()->bool {
        engine.canvas.clear();
        engine.canvas.drawBitmap1( sprite.x, sprite.y, 8, 8, heartImage );
        return true;
    } );
    sprite.x = 0;
    sprite.y = 0;
    for (int i=0; i<250; i++)
    {
        ssd1306_vga_delay(16);
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
        ssd1306_setColor(RGB_COLOR8(255,32,32));
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
    ssd1306_printFixed8(0, 16, "Bold text?", STYLE_BOLD);
    ssd1306_setColor(RGB_COLOR8(0,255,255));
    ssd1306_printFixed8(0, 24, "Italic text?", STYLE_ITALIC);
    ssd1306_negativeMode();
    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(0, 32, "Inverted bold?", STYLE_BOLD);
    ssd1306_positiveMode();
    ssd1306_vga_delay(3000);
}

// cannot use canvas due to RAM
/*static void canvasDemo()
{
    uint8_t buffer[64*16/8];
    NanoCanvas canvas(64,16, buffer);
    ssd1306_setColor(RGB_COLOR8(0,255,0));
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    canvas.clear();
    canvas.fillRect(10, 3, 80, 5, 0xFF);
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    ssd1306_vga_delay(500);
    canvas.fillRect(50, 1, 60, 15, 0xFF);
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    ssd1306_vga_delay(1500);
    canvas.printFixed(20, 1, " DEMO " );
    canvas.blt((ssd1306_displayWidth()-64)/2, 1);
    ssd1306_vga_delay(3000);
}*/

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
    ssd1306_vga_delay(3000);
}

void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_vga_controller_init();
    vga_96x40_8colors_init();
    // RGB functions do not work in default SSD1306 compatible mode
    ssd1306_setMode( LCD_MODE_NORMAL );

    ssd1306_vga_delay(3000); // wait until VGA monitor starts

    ssd1306_fillScreen8( 0x00 );
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    ssd1306_showMenu8( &menu );
}

void loop()
{
    ssd1306_vga_delay(1000);
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
//            canvasDemo();
            break;

        case 4:
            drawLinesDemo();
            break;

        default:
            break;
    }
    ssd1306_fillScreen8( 0x00 );
    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_showMenu8(&menu);
    ssd1306_vga_delay(500);
    ssd1306_menuDown(&menu);
    ssd1306_updateMenu8(&menu);
}
