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

#include "oled_ssd1331.h"
#include "lcd_common.h"
#include "ssd1331_commands.h"
#include "intf/ssd1306_interface.h"
#include "spi/ssd1306_spi.h"
#include "hal/io.h"

static const PROGMEM uint8_t s_oled96x64_initData[] =
{
    SSD1331_DISPLAYOFF,             // display off
    SSD1331_SEGREMAP, 0x00 | 0x20 | 0x10 | 0x02 | 0x01, /* 8-bit rgb color mode */
    SSD1331_SETSTARTLINE, 0x00,    // First line to start scanning from
    SSD1331_SETDISPLAYOFFSET, 0x00, // Set display offset
    SSD1331_NORMALDISPLAY,
    SSD1331_SETMULTIPLEX, 63,       // Reset to default MUX. See datasheet
    SSD1331_SETMASTER, 0x8E,        // Set master mode
    SSD1331_POWERMODE, 0x0B,        // Disable power-safe mode
    SSD1331_SETPRECHARGE, 0x31,     // Phase 1 and Phase 2 periods
    SSD1331_CLOCKDIV, 0xF0,         // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    SSD1331_PRECHARGEA, 0x64,
    SSD1331_PRECHARGEB, 0x78,
    SSD1331_PRECHARGELEVEL, 0x3A,
    SSD1331_VCOMH, 0x3E,
    SSD1331_MASTERCURRENT, 0x09,
    SSD1331_CONTRASTA, 0x91,        // RED
    SSD1331_CONTRASTB, 0x50,        // GREEN
    SSD1331_CONTRASTC, 0x7D,        // BLUE
    SSD1331_DISPLAYON,
};

static uint8_t s_column;
static uint8_t s_page;
static uint16_t s_color = 0xFF;

static void ssd1331_setBlock(uint8_t x, uint8_t y, uint8_t w)
{
    uint8_t rx = w ? (x + w - 1) : (s_displayWidth - 1);
    s_column = x;
    s_page = y;
    ssd1306_commandStart();
    ssd1306_sendByte(SSD1331_COLUMNADDR);
    ssd1306_sendByte(x);
    ssd1306_sendByte(rx < s_displayWidth ? rx : (s_displayWidth - 1));
    ssd1306_sendByte(SSD1331_ROWADDR);
    ssd1306_sendByte(y<<3);
    ssd1306_sendByte(((y<<3) + 7) < s_displayHeight ? ((y<<3) + 7) : (s_displayHeight - 1));
    ssd1306_endTransmission();
}

static void ssd1331_nextPage(void)
{
    ssd1306_endTransmission();
    ssd1331_setBlock(s_column,s_page+1,0);
    ssd1306_dataStart();
}

static void ssd1331_sendPixels(uint8_t data)
{
    for (uint8_t i=8; i>0; i--)
    {
        if ( data & 0x01 )
        {
            ssd1306_sendByte( (uint8_t)s_color );
        }
        else
        {
            ssd1306_sendByte( 0B00000000 );
        }
        data >>= 1;
    }
}

void    ssd1331_setColor(uint16_t color)
{
    s_color = color;
}

void    ssd1331_setRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
    s_color = RGB_COLOR8(r,g,b);
}

void    ssd1331_96x64_init()
{
    g_lcd_type = LCD_TYPE_SSD1331;
    s_displayHeight = 64;
    s_displayWidth = 96;
    ssd1306_setRamBlock = ssd1331_setBlock;
    ssd1306_nextRamPage = ssd1331_nextPage;
    ssd1306_sendPixels  = ssd1331_sendPixels;
    for( uint8_t i=0; i<sizeof(s_oled96x64_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled96x64_initData[i]));
    }
}

void   ssd1331_96x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 1ms after VCC is up for LCD */
        delay(1);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(10);
        digitalWrite(rstPin, HIGH);
    }
    ssd1306_spiInit(cesPin, dcPin);
    ssd1331_96x64_init();
}
