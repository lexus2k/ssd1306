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

#include "lcd_ili9341.h"
#include "lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"
#include "nano_gfx_types.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

#define CMD_ARG     0xFF

extern uint16_t ssd1306_color;
extern uint32_t s_ssd1306_spi_clock;

static uint8_t s_rotation = 0x00;
static uint8_t s_rgb_bit  = 0b00001000;
static uint8_t s_rotate_output = 0;


static const PROGMEM uint8_t s_oled240x320_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_ILI9341,
    0x00,
#endif
    0x01,                     // sw reset. not needed, we do hardware reset
    0x11,                       // exit sleep mode
    0x3A, CMD_ARG, 0x05,        // set 16-bit pixel format
    0x26, CMD_ARG, 0x04,        // set gamma curve: valid values 1, 2, 4, 8
    0xF2, CMD_ARG, 0x01,        // enable gamma adjustment, 0 - to disable
    0xE0, CMD_ARG, 0x3F, CMD_ARG, 0x25, CMD_ARG, 0x1C,
          CMD_ARG, 0x1E, CMD_ARG, 0x20, CMD_ARG, 0x12,
          CMD_ARG, 0x2A, CMD_ARG, 0x90, CMD_ARG, 0x24,
          CMD_ARG, 0x11, CMD_ARG, 0x00, CMD_ARG, 0x00,
          CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, // positive gamma correction
    0xE1, CMD_ARG, 0x20, CMD_ARG, 0x20, CMD_ARG, 0x20,
          CMD_ARG, 0x20, CMD_ARG, 0x05, CMD_ARG, 0x00,
          CMD_ARG, 0x15, CMD_ARG, 0xA7, CMD_ARG, 0x3D,
          CMD_ARG, 0x18, CMD_ARG, 0x25, CMD_ARG, 0x2A,
          CMD_ARG, 0x2B, CMD_ARG, 0x2B, CMD_ARG, 0x3A, // negative gamma correction
//    0xB1,  CMD_ARG,  0x08, CMD_ARG, 0x08, // frame rate control 1, use by default
//    0xB4,  CMD_ARG, 0x07,                 // display inversion, use by default
    0xC0,  CMD_ARG,  0x0A, CMD_ARG, 0x02, // power control 1
    0xC1,  CMD_ARG,  0x02,                // power control 2
    0xC5,  CMD_ARG,  0x50, CMD_ARG, 0x5B, // vcom control 1
    0xC7,  CMD_ARG,  0x40,                // vcom offset
    0x36,  CMD_ARG,  0b10100000,          // enable fake "vertical addressing" mode (for ili9341_setBlock() )
    0x29,                                 // display on
};

static lcduint_t s_column;
static lcduint_t s_page;

static void ili9341_setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    lcduint_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(x >> 8);
    ssd1306_intf.send(x & 0xFF);
    ssd1306_intf.send((rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1)) >> 8);
    ssd1306_intf.send((rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1)) & 0xFF);
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send((y<<3) >> 8);
    ssd1306_intf.send((y<<3)  & 0xFF);
    ssd1306_intf.send((((y<<3) + 7) < ssd1306_lcd.height ? ((y<<3) + 7) : (ssd1306_lcd.height - 1))>>8);
    ssd1306_intf.send((((y<<3) + 7) < ssd1306_lcd.height ? ((y<<3) + 7) : (ssd1306_lcd.height - 1)) & 0xFF);
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

static void ili9341_setBlock2(lcduint_t x, lcduint_t y, lcduint_t w)
{
    lcduint_t width = s_rotate_output ? ssd1306_lcd.height : ssd1306_lcd.width;
    lcduint_t height = s_rotate_output ? ssd1306_lcd.width : ssd1306_lcd.height;
    lcduint_t rx = w ? (x + w - 1) : (width - 1);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(x >> 8);
    ssd1306_intf.send(x & 0xFF);
    ssd1306_intf.send((rx < width ? rx : (width - 1))>>8);
    ssd1306_intf.send((rx < width ? rx : (width - 1)) & 0xFF);
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(y >> 8);
    ssd1306_intf.send(y & 0xFF);
    ssd1306_intf.send((height - 1) >> 8);
    ssd1306_intf.send((height - 1) & 0xFF);
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

static void ili9341_nextPage(void)
{
    ssd1306_intf.stop();
    ssd1306_lcd.set_block(s_column,s_page+1,0);
}

static void ili9341_nextPage2(void)
{
}

void    ili9341_setMode(lcd_mode_t mode)
{
    s_rotation &= 0x03;
    s_rotation |= (mode == LCD_MODE_SSD1306_COMPAT ? 0x00 : 0x04);
    ili9341_setRotation( s_rotation );
    if (mode == LCD_MODE_SSD1306_COMPAT)
    {
        ssd1306_lcd.set_block = ili9341_setBlock;
        ssd1306_lcd.next_page = ili9341_nextPage;
    }
    else if (mode == LCD_MODE_NORMAL)
    {
        ssd1306_lcd.set_block = ili9341_setBlock2;
        ssd1306_lcd.next_page = ili9341_nextPage2;
    }
}

static void ili9341_sendPixels(uint8_t data)
{
    for (uint8_t i=8; i>0; i--)
    {
        if ( data & 0x01 )
        {
            ssd1306_intf.send( (uint8_t)(ssd1306_color>>8) );
            ssd1306_intf.send( (uint8_t)(ssd1306_color) );
        }
        else
        {
            ssd1306_intf.send( 0B00000000 );
            ssd1306_intf.send( 0B00000000 );
        }
        data >>= 1;
    }
}

static void ili9341_sendPixelsBuffer(const uint8_t *buffer, uint16_t len)
{
    while(len--)
    {
        ili9341_sendPixels(*buffer);
        buffer++;
    }
}

static void ili9341_sendPixel8(uint8_t data)
{
    uint16_t color = RGB8_TO_RGB16(data);
    ssd1306_intf.send( color >> 8 );
    ssd1306_intf.send( color & 0xFF );
}

static void ili9341_sendPixel16(uint16_t color)
{
    ssd1306_intf.send( color >> 8 );
    ssd1306_intf.send( color & 0xFF );
}

void    ili9341_240x320_init()
{
    ssd1306_lcd.type = LCD_TYPE_SSD1331;
    ssd1306_lcd.width = 240;
    ssd1306_lcd.height = (lcduint_t)320;
    s_rgb_bit = 0b00001000; // set BGR mode mapping
    ssd1306_lcd.set_block = ili9341_setBlock;
    ssd1306_lcd.next_page = ili9341_nextPage;
    ssd1306_lcd.send_pixels1  = ili9341_sendPixels;
    ssd1306_lcd.send_pixels_buffer1 = ili9341_sendPixelsBuffer;
    ssd1306_lcd.send_pixels8 = ili9341_sendPixel8;
    ssd1306_lcd.send_pixels16 = ili9341_sendPixel16;
    ssd1306_lcd.set_mode = ili9341_setMode;
    ssd1306_configureSpiDisplay(s_oled240x320_initData, sizeof(s_oled240x320_initData));
}

void   ili9341_240x320_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 1ms after VCC is up for LCD */
        delay(1);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(100);
        digitalWrite(rstPin, HIGH);
        delay(100);
    }
    s_ssd1306_spi_clock = 10000000;
    ssd1306_spiInit(cesPin, dcPin);
    ili9341_240x320_init();
}

void ili9341_setRotation(uint8_t rotation)
{
    uint8_t ram_mode;
    if ((rotation^s_rotation) & 0x01)
    {
        uint16_t t = ssd1306_lcd.width;
        ssd1306_lcd.width = ssd1306_lcd.height;
        ssd1306_lcd.height = t;
    }
    s_rotation = (rotation & 0x03) | (s_rotation & 0x04);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x28);
    ssd1306_intf.send(0x36);
    ssd1306_spiDataMode(1);
    switch (s_rotation)
    {
    case 0:
        ram_mode = 0b10100000;
        break;
    case 1: // 90 degree CW
        ram_mode = 0b11010000;
        break;
    case 2: // 180 degree CW
        ram_mode = 0b01100000;
        break;
    case 3: // 270 degree CW
        ram_mode = 0b00000000;
        break;
    case 4:
        ram_mode = s_rotate_output ? 0b11100100: 0b10000100;
        break;
    case 5: // 90 degree CW
        ram_mode = 0b11100000;
        break;
    case 6: // 180 degree CW
        ram_mode = 0b01010100;
        break;
    default: // 270 degree CW
        ram_mode = 0b00100000;
        break;
    }
    ssd1306_intf.send( ram_mode | s_rgb_bit );
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x29);
    ssd1306_intf.stop();
}

void ili9341_rotateOutput(uint8_t on)
{
    s_rotate_output = on;
    ili9341_setRotation( s_rotation );
}
