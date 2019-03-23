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

#include "ssd1306_hal/io.h"
#include "ssd1306_commands.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

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

