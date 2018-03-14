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

#include "oled_ssd1351.h"
#include "lcd_common.h"
#include "ssd1351_commands.h"
#include "intf/ssd1306_interface.h"
#include "spi/ssd1306_spi.h"
#include "hal/io.h"

extern uint16_t ssd1306_color;

static const PROGMEM uint8_t s_oled128x128_initData[] =
{
    SSD1351_UNLOCK, 0xFF, 0x12,
    SSD1351_UNLOCK, 0xFF, 0xB1,
    SSD1351_SLEEP_ON,
    SSD1351_CLOCKDIV, 0xFF, 0xF1,         // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    SSD1351_SETMULTIPLEX, 0xFF, 127,      // Reset to default MUX. See datasheet
    SSD1351_SEGREMAP, 0xFF, 0B00110101,   // 16-bit rgb color mode
    SSD1351_SETSTARTLINE, 0xFF, 0x00,     // First line to start scanning from
    SSD1351_SETDISPLAYOFFSET, 0xFF, 0x00, // Set display offset
    SSD1351_SETGPIO, 0xFF, 0x00,          // GPIO OFF
    SSD1351_SETFUNCTION, 0xFF, 0x01,
    SSD1351_SETPRECHARGE, 0xFF, 0x32,     // Phase 1 and Phase 2 periods
    SSD1351_VCOMH, 0xFF, 0x05,            //
    SSD1351_PRECHARGELEVEL, 0xFF, 0x17,
    SSD1351_NORMALDISPLAY,
    SSD1351_CONTRAST,  0xFF, 0xC8,        // RED
                       0xFF, 0x80,        // GREEN
                       0xFF, 0xC8,        // BLUE
    SSD1351_MASTERCURRENT, 0xFF, 0x0F,    //
    SSD1351_EXTVSL, 0xFF, 0xA0, 0xFF, 0xB5, 0xFF, 0x55,
    SSD1351_PRECHARGESECOND, 0xFF, 0x01,  //
    SSD1351_SLEEP_OFF,                    // Disable power-safe mode
    SSD1351_NORMALDISPLAY,
};

static uint8_t s_column;
static uint8_t s_page;

static void ssd1351_setBlock(uint8_t x, uint8_t y, uint8_t w)
{
    uint8_t rx = w ? (x + w - 1) : (s_displayWidth - 1);
    s_column = x;
    s_page = y;
    ssd1306_commandStart();
    ssd1306_sendByte(SSD1351_COLUMNADDR);
    ssd1306_spiDataMode(1);
    ssd1306_sendByte(x);
    ssd1306_sendByte(rx < s_displayWidth ? rx : (s_displayWidth - 1));
    ssd1306_spiDataMode(0);
    ssd1306_sendByte(SSD1351_ROWADDR);
    ssd1306_spiDataMode(1);
    ssd1306_sendByte(y<<3);
    ssd1306_sendByte(((y<<3) + 7) < s_displayHeight ? ((y<<3) + 7) : (s_displayHeight - 1));
    ssd1306_spiDataMode(0);
    ssd1306_sendByte(0x5C);
    ssd1306_spiDataMode(1);
}

static void ssd1351_nextPage(void)
{
    ssd1306_endTransmission();
    ssd1351_setBlock(s_column,s_page+1,0);
}

static void ssd1351_sendPixels(uint8_t data)
{
    for (uint8_t i=8; i>0; i--)
    {
        if ( data & 0x01 )
        {
            ssd1306_sendByte( (uint8_t)(ssd1306_color>>8) );
            ssd1306_sendByte( (uint8_t)(ssd1306_color) );
        }
        else
        {
            ssd1306_sendByte( 0B00000000 );
            ssd1306_sendByte( 0B00000000 );
        }
        data >>= 1;
    }
}

void    ssd1351_128x128_init()
{
    g_lcd_type = LCD_TYPE_SSD1331;
    s_displayHeight = 128;
    s_displayWidth = 128;
    ssd1306_setRamBlock = ssd1351_setBlock;
    ssd1306_nextRamPage = ssd1351_nextPage;
    ssd1306_sendPixels  = ssd1351_sendPixels;
    ssd1306_commandStart();
    for( uint8_t i=0; i<sizeof(s_oled128x128_initData); i++)
    {
        uint8_t data = pgm_read_byte(&s_oled128x128_initData[i]);
        if (data == 0xFF)
        {
            data = pgm_read_byte(&s_oled128x128_initData[++i]);
            ssd1306_spiDataMode(1);
            ssd1306_sendByte(data);
            ssd1306_spiDataMode(0);
        }
        else
        {
            ssd1306_sendByte(data);
        }
    }
    ssd1306_endTransmission();
}

void   ssd1351_128x128_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
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
    ssd1306_spiInit(cesPin, dcPin);
    ssd1351_128x128_init();
}
