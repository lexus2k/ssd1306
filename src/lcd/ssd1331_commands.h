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
/**
 * @file ssd1306_commands.h SSD1331 commands definitions
 */

#ifndef _SSD1331_COMMANDS_H_
#define _SSD1331_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** SSD1331 LCD driver commands */
enum ESsd1331Commands
{
    SSD1331_COLUMNADDR       = 0x15,
    SSD1331_DRAWLINE         = 0x21,
    SSD1331_ROWADDR          = 0x75,
    SSD1331_CONTRASTA        = 0x81,
    SSD1331_CONTRASTB        = 0x82,
    SSD1331_CONTRASTC        = 0x83,
    SSD1331_MASTERCURRENT    = 0x87,
    SSD1331_PRECHARGEA       = 0x8A,
    SSD1331_PRECHARGEB       = 0x8B,
    SSD1331_SEGREMAP         = 0xA0,
    SSD1331_SETSTARTLINE     = 0xA1,
    SSD1331_SETDISPLAYOFFSET = 0xA2,
    SSD1331_NORMALDISPLAY    = 0xA4,
    SSD1331_SETMULTIPLEX     = 0xA8,
    SSD1331_SETMASTER        = 0xAD,
    SSD1331_DISPLAYOFF       = 0xAE,
    SSD1331_DISPLAYON        = 0xAF,
    SSD1331_POWERMODE        = 0xB0,
    SSD1331_SETPRECHARGE     = 0xB1,
    SSD1331_CLOCKDIV         = 0xB3,
    SSD1331_PRECHARGELEVEL   = 0xBB,
    SSD1331_VCOMH            = 0xBE,
    SSD1331_NOP              = 0xE3,
};

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1331_COMMANDS_H_
