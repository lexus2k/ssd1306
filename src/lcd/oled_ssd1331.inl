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

#include "lcd_common.h"
#include "ssd1331_commands.h"
#include "ssd1306_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif
#include "nano_gfx_types.h"
#include "oled_ssd1331.h"

static const PROGMEM uint8_t s_oled96x64_initData[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1331_X8,
    0x00,
#endif
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

static const PROGMEM uint8_t s_oled96x64_initData16[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1331_X16,
    0x00,
#endif
    SSD1331_DISPLAYOFF,             // display off
    SSD1331_SEGREMAP, 0x40 | 0x20 | 0x10 | 0x02 | 0x01, /* 16-bit rgb color mode */
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

template <class I>
void InterfaceSSD1331<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (m_base.width() - 1);
    this->start();
    spiDataMode(0);
    this->send((m_rotation & 1) ? SSD1331_ROWADDR: SSD1331_COLUMNADDR);
    this->send(x);
    this->send(rx < m_base.width() ? rx : (m_base.width() - 1));
    this->send((m_rotation & 1) ? SSD1331_COLUMNADDR: SSD1331_ROWADDR);
    this->send(y);
    this->send(m_base.height() - 1);
    spiDataMode(1);
}

template <class I>
void InterfaceSSD1331<I>::nextBlock()
{
}

template <class I>
void InterfaceSSD1331<I>::endBlock()
{
    this->stop();
}

template <class I>
void InterfaceSSD1331<I>::spiDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        digitalWrite( m_dc, mode ? HIGH : LOW );
    }
}

template <class I>
void InterfaceSSD1331<I>::commandStart(void)
{
    this->start();
    spiDataMode(0);
}

template <class I>
void InterfaceSSD1331<I>::setRotation(uint8_t rotation)
{
    uint8_t ram_mode;
    if ((rotation^m_rotation) & 0x01)
    {
        m_base.swapDimensions();
    }
    m_rotation = rotation & 0x03;
    this->start();
    spiDataMode(0);
    this->send( SSD1331_SEGREMAP );
    switch (m_rotation)
    {
    // NORMAL FULL COLOR MODE
    case 0:
        ram_mode = 0b00110010;
        break;
    case 1: // 90 degree CW
        ram_mode = 0b00110001;
        break;
    case 2: // 180 degree CW
        ram_mode = 0b00100000;
        break;
    case 3: // 270 degree CW
    default:
        ram_mode = 0b00100011;
        break;
    }
    this->send( ram_mode | (m_bits == 16 ? 0x40: 0x00) );
    this->stop();
}

template <class I>
void InterfaceSSD1331<I>::drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint16_t color)
{
    this->start();
    spiDataMode(0);
    this->send(SSD1331_DRAWLINE);
    this->send(x1);
    this->send(y1);
    this->send(x2);
    this->send(y2);
    this->send( (color & 0xF800) >> 10 );
    this->send( (color & 0x07E0) >> 5 );
    this->send( (color & 0x001F) << 1 );
    this->stop();
}

template <class I>
void InterfaceSSD1331<I>::copyBlock(uint8_t left, uint8_t top,
     uint8_t right, uint8_t bottom, uint8_t newLeft, uint8_t newTop)
{
    this->start();
    spiDataMode(0);
    this->send(0x23);
    this->send(left);
    this->send(top);
    this->send(right);
    this->send(bottom);
    this->send(newLeft);
    this->send(newTop);
    this->stop();
}


////////////////////////////////////////////////////////////////////////////////
//             SSD1331 basic 8-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I>
void DisplaySSD1331<I>::begin()
{
    if (m_rstPin >=0)
    {
        ssd1306_resetController( m_rstPin, 10 );
    }
    this->m_w = 96;
    this->m_h = 64;
    for( uint8_t i=0; i < sizeof(s_oled96x64_initData); i++)
    {
        this->m_intf.commandStart();
        this->m_intf.send(pgm_read_byte(&s_oled96x64_initData[i]));
        this->m_intf.stop();
    }
}

template <class I>
void DisplaySSD1331<I>::end()
{
}

////////////////////////////////////////////////////////////////////////////////
//             SSD1331 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I>
void DisplaySSD1331x16<I>::begin()
{
    if (m_rstPin >=0)
    {
        ssd1306_resetController( m_rstPin, 10 );
    }
    this->m_w = 96;
    this->m_h = 64;
    for( uint8_t i=0; i < sizeof(s_oled96x64_initData16); i++)
    {
        this->m_intf.commandStart();
        this->m_intf.send(pgm_read_byte(&s_oled96x64_initData16[i]));
        this->m_intf.stop();
    }
}

template <class I>
void DisplaySSD1331x16<I>::end()
{
}

