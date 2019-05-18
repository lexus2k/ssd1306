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

#include "sdl_il9163.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static uint8_t detected = 0;
static uint8_t s_lcd_type;

static int sdl_il9163_detect(uint8_t data)
{
    if (detected)
    {
        switch (data)
        {
            case 0b00000011:
                sdl_il9163.width = 128;
                sdl_il9163.height = 160;
                break;
            default:
                break;
        }
        return 1;
    }
    s_lcd_type = data;
    detected = (data == SDL_LCD_IL9163) || (data == SDL_LCD_ST7735);
    return 0;
}

static uint8_t s_verticalMode = 0;

static void sdl_il9163_commands(uint8_t data)
{
//    if ((s_verticalMode & 0b00100000) && (s_cmdArgIndex < 0))
//    {
//        if (s_commandId == 0x2A) s_commandId = 0x2B;
//        else if (s_commandId == 0x2B) s_commandId = 0x2A;
//    }
    switch (s_commandId)
    {
        case 0x36:
            if (s_cmdArgIndex == 0)
            {
                s_verticalMode = data;
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0x2A:
            switch (s_cmdArgIndex)
            {
                case 0:
                case 2:
                     break;
                case 1:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_columnStart = data;
                         s_activeColumn = data;
                     }
                     else
                     {
                         s_pageStart = data;
                         s_activePage = data;
                     }
                     break;
                case 3:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_columnEnd = data;
                     }
                     else
                     {
                         s_pageEnd = data;
                     }
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x2B:
            switch (s_cmdArgIndex)
            {
                case 0:
                case 2:
                     break;
                case 1:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         if ( s_lcd_type == SDL_LCD_ST7735 )
                             s_activePage = data;
                         else
                             // emulating bug in IL9163 Black display
                             s_activePage = (s_verticalMode & 0b10000000) ? data - 32 : data;
                         s_pageStart = s_activePage;
                     }
                     else
                     {
                         s_columnStart = data;
                         s_activeColumn = data;
                     }
                     break;
                case 3:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         if ( s_lcd_type == SDL_LCD_ST7735 )
                             s_pageEnd = data;
                         else
                             // emulating bug in IL9163 Black display
                             s_pageEnd = (s_verticalMode & 0b10000000) ? data - 32 : data;
                     }
                     else
                     {
                         s_columnEnd = data;
                     }
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x2C:
            sdl_set_data_mode( SDM_WRITE_DATA );
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xC1: // PWCTR2
        case 0xB4: // INVCTR display inversion, use by default
        case 0xC5: // VMCTR vcom control 1
        case 0x3A: // COLMOD set 16-bit pixel format
            if (s_cmdArgIndex == 0) s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xC2: // PWCTR3 power control 3
        case 0xC3: // PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
        case 0xC4: // PWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
        case 0xB6: // DISSET5
            if (s_cmdArgIndex == 1) s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xB1: // FRMCTR1 frame rate control 1, use by default
        case 0xB2: // FRMCTR2, Frame Rate Control (In Idle mode/ 8-colors)
        case 0xC0: // PWCTR1 power control 1
            if (s_cmdArgIndex == 2) s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xB3: // FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
            if (s_cmdArgIndex == 5) s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xE0: // GMCTRP1 positive gamma correction
        case 0xE1: // GMCTRN1 negative gamma correction
            if (s_cmdArgIndex == 15) s_commandId = SSD_COMMAND_NONE;
            break;
        case 0x01: // SWRESET
        case 0x11: // SLPOUT
        case 0x20: // INVOFF (20h): Display Inversion Off
        default:
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}


void sdl_il9163_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    static uint8_t firstByte = 1;  /// il9163
    static uint8_t dataFirst = 0x00;  /// il9163
    if (firstByte)
    {
        dataFirst = data;
        firstByte = 0;
        return;
    }
    firstByte = 1;
    int rx, ry;
    if (s_verticalMode & 0b00100000)
    {
        rx = (s_verticalMode & 0b01000000) ? (sdl_il9163.width - 1 - x) : x;
        ry = (s_verticalMode & 0b10000000) ? (sdl_il9163.height - 1 - y) : y;
    }
    else
    {
        rx = (s_verticalMode & 0b10000000) ? (sdl_il9163.width - 1 - x) : x;
        ry = (s_verticalMode & 0b01000000) ? (sdl_il9163.height - 1 - y) : y;
    }
    sdl_put_pixel(rx, ry, (dataFirst<<8) | data);

    if (s_verticalMode & 0b00100000)
    {
        s_activePage++;
        if (s_activePage > s_pageEnd)
        {
            s_activePage = s_pageStart;
            s_activeColumn++;
            if (s_activeColumn > s_columnEnd)
            {
                s_activeColumn = s_columnStart;
            }
        }
    }
    else
    {
        s_activeColumn++;
        if (s_activeColumn > s_columnEnd)
        {
            s_activeColumn = s_columnStart;
            s_activePage++;
            if (s_activePage > s_pageEnd)
            {
                s_activePage = s_pageStart;
            }
        }
    }
}

sdl_oled_info sdl_il9163 =
{
    .width = 128,
    .height = 128,
    .bpp = 16,
    .pixfmt = SDL_PIXELFORMAT_RGB565,
    .dataMode = SDMS_CONTROLLER,
    .detect = sdl_il9163_detect,
    .run_cmd = sdl_il9163_commands,
    .run_data = sdl_il9163_data,
};
