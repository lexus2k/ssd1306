/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

#include "ssd1306_generic.h"

extern uint8_t s_ssd1306_invertByte;

static const uint8_t PROGMEM s_oled128x64_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306,
    0x00,
#endif
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
    SSD1306_DISPLAYON,
};

static const uint8_t PROGMEM s_oled128x32_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306,
    0x00,
#endif
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
    SSD1306_DISPLAYON,
};

#if 0

static void ssd1306_setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    ssd1306_intf.start();
    if (ssd1306_intf.spi)
        ssd1306_spiDataMode(0);
    else
        ssd1306_intf.send(0x00);
    ssd1306_intf.send(SSD1306_COLUMNADDR);
    ssd1306_intf.send(x);
    ssd1306_intf.send(w ? (x + w - 1) : (ssd1306_lcd.width - 1));
    ssd1306_intf.send(SSD1306_PAGEADDR);
    ssd1306_intf.send(y);
    ssd1306_intf.send((ssd1306_lcd.height >> 3) - 1);
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

static void ssd1306_setMode_int(lcd_mode_t mode)
{
}

void ssd1306_displayOff()
{
    ssd1306_sendCommand(SSD1306_DISPLAYOFF);
}


void ssd1306_displayOn()
{
    ssd1306_sendCommand(SSD1306_DISPLAYON);
}

void ssd1306_setContrast(uint8_t contrast)
{
    ssd1306_commandStart();
    ssd1306_intf.send(SSD1306_SETCONTRAST);
    ssd1306_intf.send(contrast);
    ssd1306_intf.stop();
}

void ssd1306_invertMode()
{
    if (ssd1306_lcd.type != LCD_TYPE_SSD1331)
    {
        ssd1306_sendCommand(SSD1306_INVERTDISPLAY);
    }
}

void ssd1306_normalMode()
{
    if (ssd1306_lcd.type != LCD_TYPE_SSD1331)
    {
        ssd1306_sendCommand(SSD1306_NORMALDISPLAY);
    }
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
    ssd1306_lcd.type = LCD_TYPE_SSD1306;
    ssd1306_lcd.height = 64;
    ssd1306_lcd.width = 128;
    ssd1306_lcd.set_block = ssd1306_setBlock;
    ssd1306_lcd.next_page = ssd1306_nextPage;
    ssd1306_lcd.send_pixels1  = ssd1306_intf.send;
    ssd1306_lcd.send_pixels_buffer1 = ssd1306_intf.send_buffer;
    ssd1306_lcd.set_mode = ssd1306_setMode_int;
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
        ssd1306_resetController( rstPin, 10 );
    }
    ssd1306_spiInit(cesPin, dcPin);
    ssd1306_128x64_init();
}

///////////////////////////////////////////////////////////////////////////////
//  I2C SSD1306 128x32
///////////////////////////////////////////////////////////////////////////////

void    ssd1306_128x32_init()
{
    ssd1306_lcd.type = LCD_TYPE_SSD1306;
    ssd1306_lcd.height = 32;
    ssd1306_lcd.width = 128;
    ssd1306_lcd.set_block = ssd1306_setBlock;
    ssd1306_lcd.next_page = ssd1306_nextPage;
    ssd1306_lcd.send_pixels1  = ssd1306_intf.send;
    ssd1306_lcd.set_mode = ssd1306_setMode_int;
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

///////////////////////////////////////////////////////////////////////////////
//  SPI SSD1306 128x32
///////////////////////////////////////////////////////////////////////////////

void   ssd1306_128x32_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        ssd1306_resetController( rstPin, 10 );
    }
    ssd1306_spiInit(cesPin, dcPin);
    ssd1306_128x32_init();
}

#endif

void DisplaySSD1306::setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    m_intf.start();
    if (m_dc >= 0)
        spiDataMode(0);
    else
        m_intf.send(0x00);
    m_intf.send(SSD1306_COLUMNADDR);
    m_intf.send(x);
    m_intf.send(w ? (x + w - 1) : (m_w - 1));
    m_intf.send(SSD1306_PAGEADDR);
    m_intf.send(y);
    m_intf.send((m_h >> 3) - 1);
    if (m_dc >= 0)
    {
        spiDataMode(1);
    }
    else
    {
        m_intf.stop();
        m_intf.start();
        m_intf.send(0x40);
    }
}

void DisplaySSD1306::nextPage()
{
}

void DisplaySSD1306::spiDataMode(uint8_t mode)
{
    // DC pin
    if (m_dc >= 0)
    {
        digitalWrite( m_dc, mode ? HIGH : LOW);
    }
}

void DisplaySSD1306::commandStart(void)
{
    m_intf.start();
    if (m_dc >= 0)
        spiDataMode(0);
    else
        m_intf.send(0x00);
}

void DisplaySSD1306::invertMode()
{
    commandStart();
    m_intf.send(SSD1306_INVERTDISPLAY);
    m_intf.stop();
}

void DisplaySSD1306::normalMode()
{
    commandStart();
    m_intf.send(SSD1306_NORMALDISPLAY);
    m_intf.stop();
}

void DisplaySSD1306::setContrast(uint8_t contrast)
{
    commandStart();
    m_intf.send(SSD1306_SETCONTRAST);
    m_intf.send(contrast);
    m_intf.stop();
}

void DisplaySSD1306::displayOff()
{
    commandStart();
    m_intf.send(SSD1306_DISPLAYOFF);
    m_intf.stop();
}

void DisplaySSD1306::displayOn()
{
    commandStart();
    m_intf.send(SSD1306_DISPLAYON);
    m_intf.stop();
}

void DisplaySSD1306::flipHorizontal(uint8_t mode)
{
    commandStart();
    m_intf.send( SSD1306_SEGREMAP | (mode ? 0x00: 0x01 ) );
    m_intf.stop();
}

void DisplaySSD1306::flipVertical(uint8_t mode)
{
    commandStart();
    m_intf.send( mode ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC );
    m_intf.stop();
}

void DisplaySSD1306_128x64::begin()
{
    m_w = 128;
    m_h = 64;
    for( uint8_t i=0; i < sizeof(s_oled128x64_initData); i++)
    {
        commandStart();
        m_intf.send(pgm_read_byte(&s_oled128x64_initData[i]));
        m_intf.stop();
    }
}

void DisplaySSD1306_128x64::end()
{
}

void DisplaySSD1306_128x32::begin()
{
    m_w = 128;
    m_h = 32;
    for( uint8_t i=0; i < sizeof(s_oled128x32_initData); i++)
    {
        commandStart();
        m_intf.send(pgm_read_byte(&s_oled128x32_initData[i]));
        m_intf.stop();
    }
}

void DisplaySSD1306_128x32::end()
{
}

void DisplaySSD1306_128x64_I2C::begin()
{
    m_i2c.begin();
    DisplaySSD1306_128x64::begin();
}

void DisplaySSD1306_128x64_I2C::end()
{
    DisplaySSD1306_128x64::end();
    m_i2c.end();
}

void DisplaySSD1306_128x64_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1306_128x64::begin();
}

void DisplaySSD1306_128x64_SPI::end()
{
    DisplaySSD1306_128x64::end();
    m_spi.end();
}

