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

#include "sdl_ssd1306.h"
#include "sdl_oled_basic.h"

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;

static int sdl_ssd1306_detect(uint8_t data)
{
    return (data == 0xC0) || (data == 0xC8) || (data == 0xD5);
}

static void sdl_ssd1306_commands(uint8_t data)
{
//    printf("%02X\n", data);
    switch (s_commandId)
    {
        case 0x21:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_columnStart = ( data >= sdl_screenWidth ? sdl_screenWidth - 1: data );
                     s_activeColumn = ( data >= sdl_screenWidth ? sdl_screenWidth - 1: data );
                     break;
                case 1:
                     s_columnEnd = ( data >= sdl_screenWidth ? sdl_screenWidth - 1: data );
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x22:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_activePage = (data >= (sdl_screenHeight >> 3) ? (sdl_screenHeight >> 3) - 1 : data);
                     s_pageStart = (data >= (sdl_screenHeight >> 3) ? (sdl_screenHeight >> 3) - 1 : data);
                     break;
                case 1:
                     s_pageEnd = (data >= (sdl_screenHeight >> 3) ? (sdl_screenHeight >> 3) - 1 : data);
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0xA8:
            if (s_cmdArgIndex == 0)
            {
                sdl_ssd1306.height = data + 1;
                sdl_core_resize();
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        default:
            /* Other ssd1306 commands, many commands are combined with data */
            if ((s_commandId >= 0xb0) && (s_commandId <= 0xbf))
            {
                s_activePage =  (uint16_t)(s_commandId & 0x0F);
            }
            if ((s_commandId <= 0x0F))
            {
                s_activeColumn = (s_activeColumn & 0xFFF0) | (uint16_t)s_commandId;
            }
            if ((s_commandId <= 0x1F) && (s_commandId >= 0x10))
            {
                s_activeColumn = (s_activeColumn & 0x000F) | ((int16_t)(s_commandId & 0x0F) << 4);
            }
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}

void sdl_ssd1306_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    for (int i=0; i<8; i++)
    {
        if (data & (1<<i))
        {
            SDL_SetRenderDrawColor( g_renderer, 170, 170, 205, 255 );
        }
        else
        {
            SDL_SetRenderDrawColor( g_renderer, 20, 20, 20, 255 );
        }
        SDL_Rect r;
        r.x = x * PIXEL_SIZE + BORDER_SIZE;
        r.y = ((y<<3) + i) * PIXEL_SIZE + BORDER_SIZE + TOP_HEADER;
        r.w = PIXEL_SIZE;
        r.h = PIXEL_SIZE;
        // Render rect
        SDL_RenderFillRect( g_renderer, &r );
    }
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

sdl_oled_info sdl_ssd1306 =
{
    .width = 128,
    .height = 64,
    .dataMode = SDM_AUTO,
    .detect = sdl_ssd1306_detect,
    .run_cmd = sdl_ssd1306_commands,
    .run_data = sdl_ssd1306_data,
};
