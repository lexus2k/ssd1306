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

#include "oled_sh1106.h"
#include "lcd_common.h"
#include "ssd1306_commands.h"
#include "intf/ssd1306_interface.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif


static const uint8_t PROGMEM s_oled128x64_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SH1106,
    0x00,
#endif
    SSD1306_DISPLAYOFF, // display off
    SSD1306_COMSCANDEC,             // Scan from 127 to 0 (Reverse scan)
    SSD1306_SETSTARTLINE | 0x00,    // First line to start scanning from
    SSD1306_SETCONTRAST, 0x7F,      // contast value to 0x7F according to datasheet
    SSD1306_SEGREMAP | 0x01,        // Use reverse mapping. 0x00 - is normal mapping 
    SSD1306_NORMALDISPLAY,
    SSD1306_SETMULTIPLEX, 63,       // Reset to default MUX. See datasheet
    SSD1306_SETDISPLAYOFFSET, 0x00, // no offset
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,// set to default ratio/osc frequency
    SSD1306_SETPRECHARGE, 0x22,     // switch precharge to 0x22 // 0xF1
    SSD1306_SETCOMPINS, 0x12,       // set divide ratio
    SSD1306_SETVCOMDETECT, 0x20,    // vcom deselect to 0x20 // 0x40
    SSD1306_CHARGEPUMP, 0x14,       // Enable charge pump
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_DISPLAYON
};

static uint8_t s_column;
static uint8_t s_page;

static void sh1106_setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    if (ssd1306_intf.spi)
        ssd1306_spiDataMode(0);
    else
        ssd1306_intf.send(0x00);
    ssd1306_intf.send(SSD1306_SETPAGE | y);
    ssd1306_intf.send(((x+2)>>4) | SSD1306_SETHIGHCOLUMN);
    ssd1306_intf.send(((x+2) & 0x0f) | SSD1306_SETLOWCOLUMN);
    if (ssd1306_intf.spi)
    {
        ssd1306_spiDataMode(1);
    }
    else
    {
        ssd1306_intf.stop();
        ssd1306_intf.start();
        ssd1306_intf.send(0x40);
    }
}

static void sh1106_nextPage(void)
{
    ssd1306_intf.stop();
    sh1106_setBlock(s_column,s_page+1,0);
}

static void sh1106_setMode(lcd_mode_t mode)
{
}

void    sh1106_128x64_init()
{
    ssd1306_lcd.type = LCD_TYPE_SH1106;
    ssd1306_lcd.height = 64;
    ssd1306_lcd.width = 128;
    ssd1306_lcd.set_block = sh1106_setBlock;
    ssd1306_lcd.next_page = sh1106_nextPage;
    ssd1306_lcd.send_pixels1 = ssd1306_intf.send;
    ssd1306_lcd.send_pixels_buffer1 = ssd1306_intf.send_buffer;
    ssd1306_lcd.set_mode = sh1106_setMode;
    for( uint8_t i=0; i<sizeof(s_oled128x64_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled128x64_initData[i]));
    }
}

void    sh1106_128x64_i2c_init()
{
    ssd1306_i2cInit();
    sh1106_128x64_init();
}

void   sh1106_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        ssd1306_resetController( rstPin, 10 );
    }
    ssd1306_spiInit(cesPin, dcPin);
    sh1106_128x64_init();
}
