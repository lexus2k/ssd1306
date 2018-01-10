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
/**
 * @file ssd1306_commands.h SSD1306 commands definitions
 */

#ifndef _SSD1306_COMMANDS_H_
#define _SSD1306_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** SSD1306 LCD driver commands */
enum ESsd1306Commands
{
    SSD1306_SETLOWCOLUMN     = 0x00,
    SSD1306_SETHIGHCOLUMN    = 0x10,
    SSD1306_MEMORYMODE       = 0x20,
    SSD1306_COLUMNADDR       = 0x21,
    SSD1306_PAGEADDR         = 0x22,
    SSD1306_SETSTARTLINE     = 0x40,
    SSD1306_DEFAULT_ADDRESS  = 0x78,
    SSD1306_SETCONTRAST      = 0x81,
    SSD1306_CHARGEPUMP       = 0x8D,
    SSD1306_SEGREMAP         = 0xA0,
    SSD1306_DISPLAYALLON_RESUME = 0xA4,
    SSD1306_DISPLAYALLON     = 0xA5,
    SSD1306_NORMALDISPLAY    = 0xA6,
    SSD1306_INVERTDISPLAY    = 0xA7,
    SSD1306_SETMULTIPLEX     = 0xA8,
    SSD1306_DISPLAYOFF       = 0xAE,
    SSD1306_DISPLAYON        = 0xAF,
    SSD1306_SETPAGE          = 0xB0,
    SSD1306_COMSCANINC       = 0xC0,
    SSD1306_COMSCANDEC       = 0xC8,
    SSD1306_SETDISPLAYOFFSET = 0xD3,
    SSD1306_SETDISPLAYCLOCKDIV = 0xD5,
    SSD1306_SETPRECHARGE     = 0xD9,
    SSD1306_SETCOMPINS       = 0xDA,
    SSD1306_SETVCOMDETECT    = 0xDB,

    SSD1306_SWITCHCAPVCC     = 0x02,
    SSD1306_NOP              = 0xE3,
};

/** SSD1306 supported memory modes. */
enum ESsd1306MemoryMode
{
    HORIZONTAL_ADDRESSING_MODE  = 0x00,
    VERTICAL_ADDRESSING_MODE    = 0x01,
    PAGE_ADDRESSING_MODE        = 0x02,
};

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_COMMANDS_H_
