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

#include "sdl_ssd1331.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

#include <stdlib.h>

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static int s_newColumn;
static int s_newPage;
static uint32_t s_color = 0;

static uint8_t s_verticalMode = 1;
static uint8_t s_leftToRight = 0;
static uint8_t s_topToBottom = 0;
static uint8_t s_16bitmode = 0;
static uint8_t detected_x8 = 0;
static uint8_t detected_x16 = 0;

static void copyBlock()
{
    int x_start = s_newColumn;
    int x_end = s_newColumn + s_columnEnd - s_columnStart;
    int x_dir = s_newColumn < s_columnStart ? 1 : -1;
    int y_start = s_newPage;
    int y_end = s_newPage + s_pageEnd - s_pageStart;
    int y_dir = s_newPage < s_pageStart ? 1 : -1;


    for( int y = y_dir > 0 ? y_start: y_end;
             ((y_dir > 0) && (y <= y_end)) || ((y_dir < 0) && (y >= y_start));
             y = y + y_dir)
    {
        for( int x = x_dir > 0 ? x_start : x_end;
                 ((x_dir > 0) && (x <= x_end)) || ((x_dir < 0) && (x >= x_start));
                 x = x + x_dir)
        {
            sdl_put_pixel(x, y, sdl_get_pixel( x + s_columnStart - s_newColumn,
                                               y + s_pageStart - s_newPage ));
        }
    }
}

static void drawLine()
{
    if ( abs(s_columnStart - s_columnEnd) > abs(s_pageStart - s_pageEnd) )
    {
        int x = s_columnStart;
        while (x != s_columnEnd)
        {
            int y = s_pageStart + (s_pageEnd - s_pageStart)*(x - s_columnStart)/(s_columnEnd - s_columnStart);
            sdl_put_pixel(x, y, s_color);
            x += (s_columnEnd > s_columnStart ? 1: -1);
        }
    }
    else
    {
        int y = s_pageStart;
        while (y != s_pageEnd)
        {
            int x = s_columnStart + (s_columnEnd - s_columnStart)*(y - s_pageStart)/(s_pageEnd - s_pageStart);
            sdl_put_pixel(x, y, s_color);
            y += (s_pageEnd > s_pageStart ? 1: -1);
        }
    }
}

static void sdl_ssd1331_reset(void)
{
    detected_x8 = 0;
    detected_x16 = 0;
}

static int sdl_ssd1331_detect_x8(uint8_t data)
{
    if (detected_x8)
    {
        return 1;
    }
    detected_x8 = (data == SDL_LCD_SSD1331_X8);
    return 0;
}

static int sdl_ssd1331_detect_x16(uint8_t data)
{
    if (detected_x16)
    {
        return 1;
    }
    detected_x16 = (data == SDL_LCD_SSD1331_X16);
    return 0;
}

static void sdl_ssd1331_commands(uint8_t data)
{
    switch (s_commandId)
    {
        case 0xA0:
            if (s_cmdArgIndex == 0)
            {
                s_verticalMode = data & 0x01;
                s_leftToRight = data & 0x02;
                s_topToBottom = data & 0x10;
                s_16bitmode = data & 0x40;
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0x15:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_columnStart = data;
                     s_activeColumn = data;
                     break;
                case 1:
                     s_columnEnd = data;
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x21: // DRAW LINE
            switch (s_cmdArgIndex)
            {
                case 0: s_columnStart = data; s_color = 0; break;
                case 1: s_pageStart = data; break;
                case 2: s_columnEnd = data; break;
                case 3: s_pageEnd = data; break;
                case 4:
                    if (s_16bitmode)
                        s_color |= ((data & 0x3F) >> 1);
                    else
                        s_color |= ((data & 0x30) >> 4);
                    break;
                case 5:
                    if (s_16bitmode)
                        s_color |= ((data & 0x3F) << 5);
                    else
                        s_color |= ((data & 0x30) >> 1);
                    break;
                case 6:
                     if (s_16bitmode)
                        s_color |= ((data & 0x3E) << 10);
                     else
                        s_color |= ((data & 0x38) << 2);
                     drawLine();
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default:
                     break;
            }
            break;
        case 0x23: // MOVE BLOCK
            switch (s_cmdArgIndex)
            {
                case 0: s_columnStart = data; break;
                case 1: s_pageStart = data; break;
                case 2: s_columnEnd = data; break;
                case 3: s_pageEnd = data; break;
                case 4: s_newColumn = data; break;
                case 5:
                     s_newPage = data;
                     copyBlock();
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default:
                     break;
            }
            break;
        case 0x75:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_activePage = data;
                     s_pageStart = data;
                     break;
                case 1:
                     s_pageEnd = data;
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        default:
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}


static void sdl_ssd1331_data(uint8_t data)
{
    int y = s_topToBottom ? s_activePage : (sdl_ssd1331x8.height - s_activePage - 1);
    int x = s_leftToRight ? s_activeColumn: (sdl_ssd1331x8.width - s_activeColumn - 1);
    static uint8_t firstByte = 1;  /// SSD1331
    static uint8_t dataFirst = 0x00;  /// SSD1331
    if (firstByte && s_16bitmode)
    {
        dataFirst = data;
        firstByte = 0;
        return;
    }
    firstByte = 1;
    if ( s_16bitmode )
    {
        sdl_put_pixel(x, y, (dataFirst<<8) | data);
    }
    else
    {
        sdl_put_pixel(x, y, data);
    }

    if (s_verticalMode)
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

sdl_oled_info sdl_ssd1331x8 =
{
    .width = 96,
    .height = 64,
    .bpp = 8,
    .pixfmt = SDL_PIXELFORMAT_RGB332,
    .dataMode = SDMS_AUTO,
    .detect = sdl_ssd1331_detect_x8,
    .run_cmd = sdl_ssd1331_commands,
    .run_data = sdl_ssd1331_data,
    .reset = sdl_ssd1331_reset,
};

sdl_oled_info sdl_ssd1331x16 =
{
    .width = 96,
    .height = 64,
    .bpp = 16,
    .pixfmt = SDL_PIXELFORMAT_RGB565,
    .dataMode = SDMS_AUTO,
    .detect = sdl_ssd1331_detect_x16,
    .run_cmd = sdl_ssd1331_commands,
    .run_data = sdl_ssd1331_data,
    .reset = sdl_ssd1331_reset,
};

