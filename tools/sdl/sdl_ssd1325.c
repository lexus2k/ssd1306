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

#include "sdl_ssd1325.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static int s_newColumn;
static int s_newPage;
static uint8_t detected = 0;


static void copyBlock()
{
    if ( s_newColumn < s_columnStart )
    {
        for( int y = s_pageStart; y <= s_pageEnd; y++)
        for( int x = s_newColumn; x <= s_newColumn + s_columnEnd - s_columnStart; x++)
            sdl_put_pixel(x, y, sdl_get_pixel( x + s_columnStart - s_newColumn, y ));
    }
    else
    {
        for( int y = s_pageStart; y <= s_pageEnd; y++)
        for( int x = s_newColumn + s_columnEnd - s_columnStart; x >= s_newColumn; x--)
            sdl_put_pixel(x, y, sdl_get_pixel( x + s_columnStart - s_newColumn, y ));
    }
}

static int sdl_ssd1325_detect(uint8_t data)
{
    if (detected)
    {
        return 1;
    }
    detected = (data == SDL_LCD_SSD1325);
    return 0;
}

static uint8_t s_verticalMode = 1;
static uint8_t s_nimbleMapping = 0;
static uint8_t s_leftToRight = 0;
static uint8_t s_topToBottom = 0;

static void sdl_ssd1325_commands(uint8_t data)
{
    switch (s_commandId)
    {
        case 0xA0:
            if (s_cmdArgIndex == 0)
            {
                s_verticalMode = data & 0x04;
                s_nimbleMapping = data & 0x02;
                s_leftToRight = data & 0x01;
                s_topToBottom = data & 0x10;
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


static void sdl_ssd1325_data(uint8_t data)
{
    for (int i=0; i<2; i++)
    {
    int y = s_topToBottom ? s_activePage : (sdl_ssd1325.height - s_activePage - 1);
    int x = s_leftToRight ? s_activeColumn: (sdl_ssd1325.width - s_activeColumn - 1);
    sdl_put_pixel(x, y, ((data & 0x0F) << 28) |
                        ((data & 0x0F) << 20) |
                        ((data & 0x0F) << 12) |
                        ((data & 0x0F) << 4)
     );

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
   data >>= 4;
   }
}

sdl_oled_info sdl_ssd1325 =
{
    .width = 128,
    .height = 64,
    .bpp = 32,
    .pixfmt = SDL_PIXELFORMAT_RGBX8888,
    .dataMode = SDMS_AUTO,
    .detect = sdl_ssd1325_detect,
    .run_cmd = sdl_ssd1325_commands,
    .run_data = sdl_ssd1325_data,
};
