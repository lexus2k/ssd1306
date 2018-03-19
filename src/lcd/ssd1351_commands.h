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
 * @file ssd1351_commands.h SSD1331 commands definitions
 */

#ifndef _SSD1351_COMMANDS_H_
#define _SSD1351_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** SSD1351 LCD driver commands */
enum ESsd1351Commands
{
    SSD1351_COLUMNADDR       = 0x15,
    SSD1331_WRITEDATA        = 0x5C,
    SSD1351_ROWADDR          = 0x75,
    SSD1351_SEGREMAP         = 0xA0,
    SSD1351_SETSTARTLINE     = 0xA1,
    SSD1351_SETDISPLAYOFFSET = 0xA2,
    SSD1351_SETFUNCTION      = 0xAB,
    SSD1351_NOP              = 0xAD,
    SSD1351_ALLOFF           = 0xA4,
    SSD1351_DISPLAYON        = 0xA5,
    SSD1351_NORMALDISPLAY    = 0xA6,
    SSD1351_DISPLAYINVERSE   = 0xA7,
    SSD1351_SLEEP_ON         = 0xAE,
    SSD1351_SLEEP_OFF        = 0xAF,
    SSD1351_NOP2             = 0xB0,
    SSD1351_SETPRECHARGE     = 0xB1,
    SSD1351_CLOCKDIV         = 0xB3,
    SSD1351_EXTVSL           = 0xB4,
    SSD1351_SETGPIO          = 0xB5,
    SSD1351_PRECHARGESECOND  = 0xB6,
    SSD1351_PRECHARGELEVEL   = 0xBB,
    SSD1351_VCOMH            = 0xBE,
    SSD1351_CONTRAST         = 0xC1,
    SSD1351_MASTERCURRENT    = 0xC7,
    SSD1351_SETMULTIPLEX     = 0xCA,
    SSD1351_PRECHARGEA       = 0x8A,
    SSD1351_PRECHARGEB       = 0x8B,
    SSD1351_NOP3             = 0xE3,
    SSD1351_UNLOCK           = 0xFD,
};

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1351_COMMANDS_H_
