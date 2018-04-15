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

#include "lcd_il9163.h"
#include "lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "spi/ssd1306_spi.h"
#include "hal/io.h"
#include "nano_gfx_types.h"

#define CMD_ARG     0xFF

extern uint16_t ssd1306_color;
extern uint32_t s_ssd1306_spi_clock;

static uint8_t s_rotation = 0x00;

static const PROGMEM uint8_t s_oled128x128_initData[] =
{
//    0x01,                     // sw reset. not needed, we do hardware reset
    0x11,                       // exit sleep mode
    0x3A, CMD_ARG, 0x05,        // set 16-bit pixel format
    0x26, CMD_ARG, 0x04,        // set gamma curve: valid values 1, 2, 4, 8
//    0xF2, CMD_ARG, 0x01,        // enable gamma adjustment, 0 - to disable
//    0xE0, CMD_ARG, 0x3F, CMD_ARG, 0x25, CMD_ARG, 0x1C,
//          CMD_ARG, 0x1E, CMD_ARG, 0x20, CMD_ARG, 0x12,
//          CMD_ARG, 0x2A, CMD_ARG, 0x90, CMD_ARG, 0x24,
//          CMD_ARG, 0x11, CMD_ARG, 0x00, CMD_ARG, 0x00,
//          CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, // positive gamma correction
//    0xE1, CMD_ARG, 0x20, CMD_ARG, 0x20, CMD_ARG, 0x20,
//          CMD_ARG, 0x20, CMD_ARG, 0x05, CMD_ARG, 0x00,
//          CMD_ARG, 0x15, CMD_ARG, 0xA7, CMD_ARG, 0x3D,
//          CMD_ARG, 0x18, CMD_ARG, 0x25, CMD_ARG, 0x2A,
//          CMD_ARG, 0x2B, CMD_ARG, 0x2B, CMD_ARG, 0x3A, // negative gamma correction
//    0xB1,  CMD_ARG,  0x08, CMD_ARG, 0x08, // frame rate control 1, use by default
//    0xB4,  CMD_ARG, 0x07,                 // display inversion, use by default
    0xC0,  CMD_ARG,  0x0A, CMD_ARG, 0x02, // power control 1
    0xC1,  CMD_ARG,  0x02,                // power control 2
    0xC5,  CMD_ARG,  0x50, CMD_ARG, 0x5B, // vcom control 1
    0xC7,  CMD_ARG,  0x40,                // vcom offset
//    0x2A,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x7F,   // set column address, not needed. set by direct API
//    0x2B,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x9F,   // set page address, not needed. set by direct API
    0x36,  CMD_ARG,  0b00101000,          // enable fake "vertical addressing" mode (for il9163_setBlock() )
    0x29,                                 // display on
};

static uint8_t s_column;
static uint8_t s_page;

static void il9163_setBlock(uint8_t x, uint8_t y, uint8_t w)
{
    uint8_t rx = w ? (x + w - 1) : (s_displayWidth - 1);
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(x + (s_rotation == 3 ? 32 : 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send((rx < s_displayWidth ? rx : (s_displayWidth - 1))
                       + (s_rotation == 3 ? 32 : 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send((y<<3) + (s_rotation == 2 ? 32: 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send((((y<<3) + 7) < s_displayHeight ? ((y<<3) + 7) : (s_displayHeight - 1))
                      + (s_rotation == 2 ? 32: 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

static void il9163_setBlock2(uint8_t x, uint8_t y, uint8_t w)
{
    uint8_t rx = w ? (x + w - 1) : (s_displayWidth - 1);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(x + (s_rotation == 7 ? 32 : 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send((rx < s_displayWidth ? rx : (s_displayWidth - 1))
                       + (s_rotation == 7 ? 32 : 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(y + (s_rotation == 6 ? 32: 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send(s_displayHeight - 1 + (s_rotation == 6 ? 32: 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

static void il9163_nextPage(void)
{
    ssd1306_intf.stop();
    il9163_setBlock(s_column,s_page+1,0);
}

void    il9163_setMode(uint8_t vertical)
{
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send( 0x36 );
    ssd1306_spiDataMode(1);
    ssd1306_intf.send( vertical ? 0b00101000 : 0b00001000 );
    ssd1306_intf.stop();
    if (vertical)
    {
        ssd1306_setRamBlock = il9163_setBlock;
    }
    else
    {
        ssd1306_setRamBlock = il9163_setBlock2;
    }
    s_rotation = vertical ? 0x00 : 0x04;
}

static void il9163_sendPixels(uint8_t data)
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

void        il9163_setRotation(uint8_t rotation)
{
    if ((rotation^s_rotation) & 0x01)
    {
        uint16_t t = s_displayWidth;
        s_displayWidth = s_displayHeight;
        s_displayHeight = t;
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
        ssd1306_intf.send( 0b00101000 );
        break;
    case 1: // 90 degree CW
        ssd1306_intf.send( 0b01001000 );
        break;
    case 2: // 180 degree CW
        ssd1306_intf.send( 0b11101000 );
        break;
    case 3: // 270 degree CW
        ssd1306_intf.send( 0b10001000 );
        break;
    case 4:
        ssd1306_intf.send( 0b00001000 );
        break;
    case 5: // 90 degree CW
        ssd1306_intf.send( 0b01101000 );
        break;
    case 6: // 180 degree CW
        ssd1306_intf.send( 0b11001000 );
        break;
    default: // 270 degree CW
        ssd1306_intf.send( 0b10101000 );
        break;
    }
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x29);
    ssd1306_intf.stop();
}


static void il9163_sendPixelsBuffer(const uint8_t *buffer, uint16_t len)
{
    while(len--)
    {
        il9163_sendPixels(*buffer);
        buffer++;
    }
}

static void il9163_sendPixel8(uint8_t data)
{
    uint16_t color = RGB8_TO_RGB16(data);
    ssd1306_intf.send( color >> 8 );
    ssd1306_intf.send( color & 0xFF );
}

void    il9163_128x128_init()
{
    g_lcd_type = LCD_TYPE_SSD1331;
    s_displayHeight = 128;
    s_displayWidth = 128;
    ssd1306_setRamBlock = il9163_setBlock;
    ssd1306_nextRamPage = il9163_nextPage;
    ssd1306_sendPixels  = il9163_sendPixels;
    ssd1306_sendPixelsBuffer = il9163_sendPixelsBuffer;
    ssd1306_sendPixel8 = il9163_sendPixel8;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    for( uint8_t i=0; i<sizeof(s_oled128x128_initData); i++)
    {
        uint8_t data = pgm_read_byte(&s_oled128x128_initData[i]);
        if (data == CMD_ARG)
        {
            data = pgm_read_byte(&s_oled128x128_initData[++i]);
            ssd1306_spiDataMode(1);
            ssd1306_intf.send(data);
            ssd1306_spiDataMode(0);
        }
        else
        {
            ssd1306_intf.send(data);
        }
    }
    ssd1306_intf.stop();
}

void   il9163_128x128_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 1ms after VCC is up for LCD */
        delay(1);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(20);
        digitalWrite(rstPin, HIGH);
    }
    /* ssd1351 cannot work faster than at 4MHz per datasheet */
    s_ssd1306_spi_clock = 8000000;
    ssd1306_spiInit(cesPin, dcPin);
    il9163_128x128_init();
}
