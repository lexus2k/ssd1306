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

#include "oled_ssd1306.h"
#include "lcd_common.h"
#include "ssd1306_commands.h"
#include "intf/ssd1306_interface.h"
#include "i2c/ssd1306_i2c.h"
#include "spi/ssd1306_spi.h"
#include "hal/io.h"

static const uint8_t PROGMEM s_oled128x64_initData[] =
{
    SSD1306_DISPLAYOFF, // display off
    SSD1306_MEMORYMODE, HORIZONTAL_ADDRESSING_MODE, // Page Addressing mode
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

static const uint8_t PROGMEM s_oled128x32_initData[] =
{
    SSD1306_DISPLAYOFF, // display off
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,
    SSD1306_SETMULTIPLEX, 31,
    SSD1306_SETDISPLAYOFFSET, 0x00, // --no offset
    SSD1306_SETSTARTLINE,
    SSD1306_CHARGEPUMP, 0x14, // 0x10
    SSD1306_SEGREMAP | 0x01,  // Reverse mapping
    SSD1306_COMSCANDEC,
    SSD1306_SETCOMPINS, 0x02,
    SSD1306_SETCONTRAST, 0x7F, // contast value
    SSD1306_SETPRECHARGE, 0x22, // 0x1F
    SSD1306_SETVCOMDETECT, 0x40,
    SSD1306_MEMORYMODE, HORIZONTAL_ADDRESSING_MODE,
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    SSD1306_DISPLAYON 
};

static void ssd1306_setBlock(uint8_t x, uint8_t y, uint8_t w)
{
    ssd1306_intf.start();
    if (ssd1306_intf.spi)
        ssd1306_spiDataMode(0);
    else
        ssd1306_intf.send(0x00);
    ssd1306_intf.send(SSD1306_COLUMNADDR);
    ssd1306_intf.send(x);
    ssd1306_intf.send(w ? (x + w - 1) : (s_displayWidth - 1));
    ssd1306_intf.send(SSD1306_PAGEADDR);
    ssd1306_intf.send(y);
    ssd1306_intf.send((s_displayHeight >> 3) - 1);
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

static void ssd1306_nextPage(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//  I2C SSD1306 128x64
///////////////////////////////////////////////////////////////////////////////

void    ssd1306_init()
{
    ssd1306_128x64_i2c_init();
}


void    ssd1306_128x64_init()
{
    g_lcd_type = LCD_TYPE_SSD1306;
    s_displayHeight = 64;
    s_displayWidth = 128;
    ssd1306_setRamBlock = ssd1306_setBlock;
    ssd1306_nextRamPage = ssd1306_nextPage;
    ssd1306_sendPixels  = ssd1306_intf.send;
    ssd1306_sendPixelsBuffer = ssd1306_intf.send_buffer;
    for( uint8_t i=0; i<sizeof(s_oled128x64_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled128x64_initData[i]));
    }
}

void    ssd1306_128x64_i2c_init()
{
    ssd1306_i2cInit();
    ssd1306_128x64_init();
}

void    ssd1306_128x64_i2c_initEx(int8_t scl, int8_t sda, int8_t sa)
{
    ssd1306_i2cInitEx(scl, sda, sa);
    ssd1306_128x64_init();
}

///////////////////////////////////////////////////////////////////////////////
//  SPI SSD1306 128x64
///////////////////////////////////////////////////////////////////////////////

void   ssd1306_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
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
    ssd1306_128x64_init();
}

///////////////////////////////////////////////////////////////////////////////
//  I2C SSD1306 128x32
///////////////////////////////////////////////////////////////////////////////

void    ssd1306_128x32_init()
{
    g_lcd_type = LCD_TYPE_SSD1306;
    s_displayHeight = 32;
    s_displayWidth = 128;
    ssd1306_setRamBlock = ssd1306_setBlock;
    ssd1306_nextRamPage = ssd1306_nextPage;
    ssd1306_sendPixels  = ssd1306_intf.send;
    for( uint8_t i=0; i < sizeof(s_oled128x32_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled128x32_initData[i]));
    }
}


void    ssd1306_128x32_i2c_init()
{
    ssd1306_i2cInit();
    ssd1306_128x32_init();
}
