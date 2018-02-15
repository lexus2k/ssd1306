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

enum
{
    SDL_AUTODETECT,
    SDL_SSD1306,
    SDL_SSD1331,
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
static int s_commandMode = 0;
static int m_columnStartAddress = 0;
static int m_columnEndAddress = 127;
static int m_pageStartAddress = 0;
static int m_pageEndAddress = 7;
static int m_activeColumn = 0;
static int m_activePage = 0;
static int m_command;

//static uint8_t  m_buffer[64][128];

void sdl_send_init()
{
    s_commandMode = -1;
    m_command = -1;
}

void sdl_data_start()
{
    s_commandMode = 0;
    m_command = -1;
}

void sdl_command_start()
{
    s_commandMode = 1;
    m_command = -1;
}


static void sdl_ssd1306_commands(uint8_t data);
static void sdl_ssd1331_commands(uint8_t data);

void sdl_send_byte(uint8_t data)
{
    if (s_commandMode < 0)
    {
        s_commandMode = data == 0x00 ? 1 : 0;
    }
    else
    {
        if (s_oled == SDL_AUTODETECT)
        {
            if (data == 0xBE) s_oled = SDL_SSD1331;
            if ((data == 0xC0) || (data == 0xC8)) s_oled = SDL_SSD1306;
        }
        if (s_oled == SDL_SSD1306)
        {
            sdl_ssd1306_commands(data);
        }
        if (s_oled == SDL_SSD1331)
        {
            sdl_ssd1331_commands(data);
        }
    }
}

void sdl_send_stop()
{
    if (!s_commandMode)
    {
        sdl_core_draw();
    }
    s_commandMode = -1;
}


static void sdl_ssd1306_commands(uint8_t data)
{
    if (s_commandMode)
    {
        //printf("%02X\n", data);
        if ((m_command & 0x00FF) == 0x21)
        {
            if ((m_command & 0x0100) == 0x0000)
            {
                m_columnStartAddress = data;
                m_activeColumn = data;
                m_command |= 0x0100;
                //printf("[column:%d- ]\n", m_columnStartAddress);
                return;
            }
            else
            {
                m_columnEndAddress = data;
                m_command = -1;
                //printf("[column: -%d]\n", m_columnEndAddress);
                return;
            }
        }
        else if ((m_command & 0x00FF) == 0x22)
        {
            if ((m_command & 0x0100) == 0x0000)
            {
                m_activePage = data;
                m_pageStartAddress = data;
                m_command |= 0x0100;
                //printf("[page:%d- ]\n", m_pageStartAddress);
                return;
            }
            else
            {
                m_pageEndAddress = data;
                m_command = -1;
                //printf("[page: -%d]\n", m_pageEndAddress);
                return;
            }
        }
        else
        {
            if ((data >= 0xb0) && (data <= 0xbf))
            {
                m_activePage =  (uint16_t)(data & 0x0F);
            }
            else if ((data <= 0x0F))
            {
                m_activeColumn = (m_activeColumn & 0xFFF0) | (uint16_t)data;
            }
            else if ((data <= 0x1F) && (data >= 0x10))
            {
                m_activeColumn = (m_activeColumn & 0x000F) | ((int16_t)(data & 0x0F) << 4);
            }
            else
            {
                m_command = data;
            }
        }
    }
    else
    {
        int y = m_activePage;
        int x = m_activeColumn;
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
        m_activeColumn++;
        if (m_activeColumn > m_columnEndAddress)
        {
            m_activeColumn = m_columnStartAddress;
            m_activePage++;
            if (m_activePage > m_pageEndAddress)
            {
                m_activePage = m_pageStartAddress;
            }
        }
    }
}


static void sdl_ssd1331_commands(uint8_t data)
{
    if (s_commandMode)
    {
        //printf("%02X\n", data);
        if ((m_command & 0x00FF) == 0x15)
        {
            if ((m_command & 0x0100) == 0x0000)
            {
                m_columnStartAddress = data;
                m_activeColumn = data;
                m_command |= 0x0100;
                //printf("[column:%d- ]\n", m_columnStartAddress);
                return;
            }
            else
            {
                m_columnEndAddress = data;
                m_command = -1;
                //printf("[column: -%d]\n", m_columnEndAddress);
                return;
            }
        }
        else if ((m_command & 0x00FF) == 0x75)
        {
            if ((m_command & 0x0100) == 0x0000)
            {
                m_activePage = data;
                m_pageStartAddress = data;
                m_command |= 0x0100;
                //printf("[page:%d- ]\n", m_pageStartAddress);
                return;
            }
            else
            {
                m_pageEndAddress = data;
                m_command = -1;
                //printf("[page: -%d]\n", m_pageEndAddress);
                return;
            }
        }
        else
        {
//            if ((data <= 0x1F) && (data >= 0x10))
//            {
//                m_activeColumn = (m_activeColumn & 0x000F) | ((int16_t)(data & 0x0F) << 4);
//            }
//            else
            {
                m_command = data;
            }
        }
    }
    else
    {
        int y = m_activePage;
        int x = m_activeColumn;
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

        m_activePage++;
        if (m_activePage > m_pageEndAddress)
        {
            m_activePage = m_pageStartAddress;
            m_activeColumn++;
            if (m_activeColumn > m_columnEndAddress)
            {
                m_activeColumn = m_columnStartAddress;
            }
        }
    }
}

