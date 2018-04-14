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
#include "sdl_oled_basic.h"
#include "sdl_ssd1306.h"
#include "sdl_ssd1331.h"
#include "sdl_ssd1351.h"
#include "sdl_il9163.h"
#include "sdl_pcd8544.h"
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#define CANVAS_REFRESH_RATE  60

enum
{
    SDL_AUTODETECT,
    SDL_DETECTED,
};

int sdl_screenWidth = 128;
int sdl_screenHeight = 64;

SDL_Window     *g_window = NULL;
SDL_Renderer   *g_renderer = NULL;
SDL_Texture    *g_texture = NULL;
void           *g_pixels = NULL;

static int s_analogInput[128];
static int s_digitalPins[128];
static int s_dcPin = -1;

static sdl_oled_info *p_oled_db[128] = { NULL };
static sdl_oled_info *p_active_driver = NULL;

static int windowWidth() { return sdl_screenWidth * PIXEL_SIZE + BORDER_SIZE * 2; };
static int windowHeight() { return sdl_screenHeight * PIXEL_SIZE + BORDER_SIZE * 2 + TOP_HEADER; };

static void register_oled(sdl_oled_info *oled_info)
{
    sdl_oled_info **p = p_oled_db;
    while (*p) (p++);
    *p = oled_info;
}

void sdl_core_init(void)
{
    register_oled( &sdl_ssd1306 );
    register_oled( &sdl_ssd1331 );
    register_oled( &sdl_ssd1351 );
    register_oled( &sdl_il9163 );
    register_oled( &sdl_pcd8544 );
    if ((g_window != NULL) && (g_renderer != NULL))
    {
         /* SDL engine is already initialize */
         return;
    }
    SDL_Init(SDL_INIT_EVERYTHING);
    g_window = SDL_CreateWindow
    (
        "AVR SIMULATOR", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        windowWidth(),
        windowHeight(),
        SDL_WINDOW_SHOWN
    );
    g_renderer =  SDL_CreateRenderer( g_window, -1, SDL_RENDERER_ACCELERATED );
    // Set render color to black ( background will be rendered in this color )
    SDL_SetRenderDrawColor( g_renderer, 20, 20, 20, 255 );

    // Clear window
    SDL_RenderClear( g_renderer );

    // Render the rect to the screen
    SDL_RenderPresent(g_renderer);

    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = windowWidth();
    r.h = windowHeight();

    SDL_RenderFillRect( g_renderer, &r );
}

static void sdl_poll_event(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) exit(0);
        switch (event.type)
        {
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)  s_analogInput[0] = 300;
                if (event.key.keysym.scancode == SDL_SCANCODE_UP)    s_analogInput[0] = 150;
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)  s_analogInput[0] = 500;
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) s_analogInput[0] = 50;
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) s_analogInput[0] = 700;
                break;
            case SDL_KEYUP:
                if ((event.key.keysym.scancode == SDL_SCANCODE_DOWN)  ||
                    (event.key.keysym.scancode == SDL_SCANCODE_UP) ||
                    (event.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
                    (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
                    (event.key.keysym.scancode == SDL_SCANCODE_SPACE)) s_analogInput[0] = 1023;
                break;
            default:
                break;
        };
    }
}

void sdl_set_dc_pin(int pin)
{
    s_dcPin = pin;
}

int sdl_read_analog(int pin)
{
    sdl_poll_event();
    return s_analogInput[pin];
}

void sdl_write_digital(int pin, int value)
{
    s_digitalPins[pin] = value;
}

static void sdl_draw_oled_frame(void)
{
    SDL_Rect r;
    SDL_SetRenderDrawColor( g_renderer, 60, 128, 192, 255 );

    r.x = 0; r.y = 0;
    r.w = windowWidth();
    r.h = TOP_HEADER + BORDER_SIZE - RECT_THICKNESS;
    SDL_RenderFillRect( g_renderer, &r );

    r.x = 0; r.y = TOP_HEADER + BORDER_SIZE - RECT_THICKNESS;
    r.w = BORDER_SIZE - RECT_THICKNESS;
    r.h = windowHeight() - ((BORDER_SIZE - RECT_THICKNESS)*2 + TOP_HEADER);
    SDL_RenderFillRect( g_renderer, &r );

    r.x = windowWidth() - (BORDER_SIZE - RECT_THICKNESS); r.y = TOP_HEADER + BORDER_SIZE - RECT_THICKNESS;
    r.w = BORDER_SIZE - RECT_THICKNESS;
    r.h = windowHeight() - ((BORDER_SIZE - RECT_THICKNESS)*2 + TOP_HEADER);
    SDL_RenderFillRect( g_renderer, &r );

    r.x = 0; r.y = windowHeight() - ((BORDER_SIZE - RECT_THICKNESS));
    r.w = windowWidth();
    r.h = BORDER_SIZE - RECT_THICKNESS;
    SDL_RenderFillRect( g_renderer, &r );

    SDL_SetRenderDrawColor( g_renderer, 0, 0, 0, 255 );
    for (int i=0; i<RECT_THICKNESS; i++)
    {
       r.x = BORDER_SIZE - RECT_THICKNESS + i;
       r.y = BORDER_SIZE - RECT_THICKNESS + TOP_HEADER + i;
       r.w = windowWidth() - (BORDER_SIZE - RECT_THICKNESS + i)*2;
       r.h = windowHeight() - ((BORDER_SIZE - RECT_THICKNESS + i)*2 + TOP_HEADER);
       SDL_RenderDrawRect( g_renderer, &r );
    }

#if !defined(SDL_NO_BORDER)
    SDL_SetRenderDrawColor( g_renderer, 200, 200, 200, 255 );
    r.x = 4;
    r.y = 4;
    r.w = 16;
    r.h = 12;
    SDL_RenderFillRect( g_renderer, &r );
    r.x = windowWidth() - 4 - 16;
    r.y = 4;
    r.w = 16;
    r.h = 12;
    SDL_RenderFillRect( g_renderer, &r );
#endif
}

void sdl_core_draw(void)
{
    sdl_poll_event();
    sdl_draw_oled_frame();
    if (g_texture && p_active_driver)
    {
        SDL_Rect r;
        void * l_pixels;
        int  pitch;
        if (SDL_LockTexture(g_texture, NULL, &l_pixels, &pitch) == 0)
        {
            if (pitch != sdl_screenWidth * (p_active_driver->bpp/8))
            {
                fprintf(stderr, "Warning, pitch %d is not expected\n", pitch);
            }
            memcpy(l_pixels, g_pixels, sdl_screenWidth * sdl_screenHeight * (p_active_driver->bpp/8));
            SDL_UnlockTexture(g_texture);
        }
        else
        {
            fprintf(stderr, "Something bad happened to SDL texture\n");
            exit(1);
        }
        r.x = BORDER_SIZE;
        r.y = BORDER_SIZE + TOP_HEADER;
        r.w = windowWidth() - BORDER_SIZE * 2;
        r.h = windowHeight() - BORDER_SIZE * 2 - TOP_HEADER;
        SDL_RenderCopy(g_renderer, g_texture, NULL, &r);
    }
    SDL_RenderPresent(g_renderer);
}

void sdl_core_close(void)
{
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

void sdl_core_resize(void)
{
    SDL_Rect r;
    if (p_active_driver)
    {
        sdl_screenWidth = p_active_driver->width;
        sdl_screenHeight = p_active_driver->height;
        if (g_texture)
        {
            SDL_DestroyTexture( g_texture );
            g_texture = NULL;
            free(g_pixels);
            g_pixels = NULL;
        }
        g_pixels = malloc(sdl_screenWidth * sdl_screenHeight * (p_active_driver->bpp/8));
        g_texture = SDL_CreateTexture( g_renderer, p_active_driver->pixfmt,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       p_active_driver->width, p_active_driver->height );
        if (g_texture == NULL)
        {
            fprintf(stderr, "Error creating back buffer: %s\n", SDL_GetError());
            exit(1);
        }
    }
    SDL_SetWindowSize(g_window, windowWidth(), windowHeight());

    SDL_SetRenderDrawColor( g_renderer, 20, 20, 20, 255 );
    r.x = RECT_THICKNESS;
    r.y = RECT_THICKNESS;
    r.w = windowWidth() - RECT_THICKNESS*2;
    r.h = windowHeight() - RECT_THICKNESS*2;
    SDL_RenderFillRect( g_renderer, &r );
    sdl_draw_oled_frame();
}

void sdl_put_pixel(int x, int y, uint32_t color)
{
    while (x >= sdl_screenWidth) x-= sdl_screenWidth;
    while (y >= sdl_screenHeight) y-= sdl_screenHeight;
    if (x<0) x = 0;
    if (y<0) y = 0;
    if (g_pixels && p_active_driver)
    {
        int index = x + y * sdl_screenWidth;
        switch (p_active_driver->bpp)
        {
            case 8:
                ((uint8_t *)g_pixels)[ index ] = color;
                break;
            case 16:
                ((uint16_t *)g_pixels)[ index ] = color;
                break;
            case 32:
                ((uint32_t *)g_pixels)[ index ] = color;
                break;
            default:
                break;
        }
    }
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int s_commandId;
int s_cmdArgIndex;
static int s_ssdMode = SSD_MODE_NONE;
static sdl_data_mode s_active_data_mode = SDM_COMMAND_ARG;

static int s_oled = SDL_AUTODETECT;

void sdl_send_init()
{
    s_active_data_mode = SDM_COMMAND_ARG;
    s_ssdMode = SSD_MODE_NONE;
    s_commandId = SSD_COMMAND_NONE;
}


static void sdl_send_command_or_arg(uint8_t data)
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
    if (p_active_driver)
    {
        p_active_driver->run_cmd( data );
    }
}

static void sdl_write_data(uint8_t data)
{
    if (p_active_driver)
    {
        p_active_driver->run_data( data );
    }
}

void sdl_send_byte(uint8_t data)
{
    if (s_dcPin>=0)
    {
        // for spi
        s_ssdMode = s_digitalPins[s_dcPin] ? SSD_MODE_DATA : SSD_MODE_COMMAND;
    }
    else if (s_ssdMode == SSD_MODE_NONE)
    {
        // for i2c
        s_ssdMode = data == 0x00 ? SSD_MODE_COMMAND : SSD_MODE_DATA;
        return;
    }
    if (s_ssdMode == SSD_MODE_COMMAND)
    {
        if (s_oled == SDL_AUTODETECT)
        {
            sdl_oled_info **p = p_oled_db;
            while (*p)
            {
                if ((*p)->detect(data))
                {
                    p_active_driver = *p;
                    s_oled = SDL_DETECTED;
                    sdl_core_resize();
                    break;
                }
                p++;
            }
        }
        else
        {
            sdl_send_command_or_arg( data );
        }
    }
    else
    {
        if (p_active_driver)
        {
            if (p_active_driver->dataMode == SDMS_AUTO)
            {
                s_active_data_mode = SDM_WRITE_DATA;
            }
            switch ( s_active_data_mode )
            {
                case SDM_COMMAND_ARG:
                    sdl_send_command_or_arg( data );
                    break;
                case SDM_WRITE_DATA:
                    sdl_write_data( data );
                    break;
                default:
                    break;
            }
        }
    }
}

void sdl_send_stop()
{
    sdl_core_draw();
    s_ssdMode = -1;
}

void sdl_set_data_mode(sdl_data_mode mode)
{
    s_active_data_mode = mode;
}


