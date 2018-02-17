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

#include "sdl_core.h"
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#define PIXEL_SIZE 2
#define CANVAS_REFRESH_RATE  60

#define SSD_COMMAND_NONE -1

enum
{
    SDL_AUTODETECT,
    SDL_SSD1306,
    SDL_SSD1331,
};

enum
{
    SSD_MODE_NONE,
    SSD_MODE_COMMAND,
    SSD_MODE_DATA,
};

const static int screenWidth = 258;
const static int screenHeight = 130;
SDL_Window     *g_window = NULL;
SDL_Renderer   *g_renderer = NULL;

void sdl_core_init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    g_window = SDL_CreateWindow
    (
        "AVR SIMULATOR", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screenWidth,
        screenHeight,
        SDL_WINDOW_SHOWN
    );
    g_renderer =  SDL_CreateRenderer( g_window, -1, SDL_RENDERER_ACCELERATED);
    // Set render color to black ( background will be rendered in this color )
    SDL_SetRenderDrawColor( g_renderer, 20, 20, 20, 255 );

    // Clear window
    SDL_RenderClear( g_renderer );

    // Render the rect to the screen
    SDL_RenderPresent(g_renderer);

    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = screenWidth;
    r.h = screenHeight;

    SDL_RenderFillRect( g_renderer, &r );
}

void sdl_core_draw(void)
{
    SDL_Event event;
    SDL_RenderPresent(g_renderer);
    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) exit(0);
    }
}

void sdl_core_close(void)
{
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

static int s_oled = SDL_AUTODETECT;
static int s_ssdMode = SSD_MODE_NONE;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_commandId;
static int s_cmdArgIndex;

void sdl_send_init()
{
    s_ssdMode = -1;
    s_commandId = SSD_MODE_NONE;
}

void sdl_data_start()
{
    s_ssdMode = SSD_MODE_DATA;
    s_commandId = -1;
}

void sdl_command_start()
{
    s_ssdMode = SSD_MODE_COMMAND;
    s_commandId = -1;
}


static void sdl_ssd1306_commands(uint8_t data);
static void sdl_ssd1306_data(uint8_t data);
static void sdl_ssd1331_commands(uint8_t data);
static void sdl_ssd1331_data(uint8_t data);

void sdl_send_byte(uint8_t data)
{
    if (s_ssdMode == SSD_MODE_NONE)
    {
        s_ssdMode = data == 0x00 ? SSD_MODE_COMMAND : SSD_MODE_DATA;
    }
    else if (s_ssdMode == SSD_MODE_COMMAND)
    {
        if (s_oled == SDL_AUTODETECT)
        {
            if (data == 0xBE) s_oled = SDL_SSD1331;
            if ((data == 0xC0) || (data == 0xC8)) s_oled = SDL_SSD1306;
        }
        else
        {
            if (s_commandId == SSD_COMMAND_NONE)
            {
                s_commandId = data;
                s_cmdArgIndex = -1; // no argument
            }
            else
            {
                s_cmdArgIndex++;
            }
            switch (s_oled)
            {
                case SDL_SSD1306: sdl_ssd1306_commands(data); break;
                case SDL_SSD1331: sdl_ssd1331_commands(data); break;
                default: break;
            }
        }
    }
    else
    {
        switch (s_oled)
        {
            case SDL_SSD1306: sdl_ssd1306_data(data); break;
            case SDL_SSD1331: sdl_ssd1331_data(data); break;
            default: break;
        }
    }
}

void sdl_send_stop()
{
    if (!s_ssdMode)
    {
        sdl_core_draw();
    }
    s_ssdMode = -1;
}


static void sdl_ssd1306_commands(uint8_t data)
{
    //printf("%02X\n", data);
    switch (s_commandId)
    {
        case 0x21:
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
        case 0x22:
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
        r.x = x * PIXEL_SIZE + 1;
        r.y = ((y<<3) + i) * PIXEL_SIZE + 1;
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


static void sdl_ssd1331_commands(uint8_t data)
{
    switch (s_commandId)
    {
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


void sdl_ssd1331_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    SDL_SetRenderDrawColor( g_renderer, (data & 0b11100000)<<0,
                                        (data & 0b00011100)<<3,
                                        (data & 0b00000011)<<6,
                                        255 );
    SDL_Rect r;
    r.x = x * PIXEL_SIZE + 1;
    r.y = y * PIXEL_SIZE + 1;
    r.w = PIXEL_SIZE;
    r.h = PIXEL_SIZE;
    // Render rect
    SDL_RenderFillRect( g_renderer, &r );

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

