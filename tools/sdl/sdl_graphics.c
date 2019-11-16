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

#include "sdl_graphics.h"
#include "sdl_oled_basic.h"
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define CANVAS_REFRESH_RATE  60

static SDL_Window     *g_window = NULL;
static SDL_Renderer   *g_renderer = NULL;
static SDL_Texture    *g_texture = NULL;
void                  *g_pixels = NULL;

static int s_width = 128;
static int s_height = 64;
static int s_bpp = 16;
static uint32_t s_pixfmt = SDL_PIXELFORMAT_RGB565;
static bool s_unittest_mode = false;

static int windowWidth() { return s_width * PIXEL_SIZE + BORDER_SIZE * 2; };
static int windowHeight() { return s_height * PIXEL_SIZE + BORDER_SIZE * 2 + TOP_HEADER; };

void sdl_graphics_init(void)
{
    if ((g_window != NULL) && (g_renderer != NULL))
    {
        /* SDL engine is already initialize */
        return;
    }
    if ( s_unittest_mode )
    {
        SDL_Init(0);
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

void sdl_graphics_refresh(void)
{
    if ( s_unittest_mode )
    {
        return;
    }
    sdl_draw_oled_frame();
    if (g_texture)
    {
        SDL_Rect r;
        void * l_pixels;
        int  pitch;
        if (SDL_LockTexture(g_texture, NULL, &l_pixels, &pitch) == 0)
        {
            if (pitch != s_width * (s_bpp/8))
            {
                fprintf(stderr, "Warning, pitch %d is not expected\n", pitch);
            }
            memcpy(l_pixels, g_pixels, s_width * s_height * (s_bpp/8));
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

void sdl_graphics_set_oled_params(int width, int height, int bpp, uint32_t pixfmt)
{
    SDL_Rect r;
    s_bpp = bpp;
    s_pixfmt = pixfmt;
    s_width = width;
    s_height = height;
    if (g_texture)
    {
        SDL_DestroyTexture( g_texture );
        g_texture = NULL;
        free(g_pixels);
        g_pixels = NULL;
    }
    g_pixels = malloc(s_width * s_height * (s_bpp / 8));
    if ( s_unittest_mode )
    {
        return;
    }
    g_texture = SDL_CreateTexture( g_renderer, s_pixfmt,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   width, height );
    if (g_texture == NULL)
    {
        fprintf(stderr, "Error creating back buffer: %s\n", SDL_GetError());
        exit(1);
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
    while (x >= s_width) x-= s_width;
    while (y >= s_height) y-= s_height;
    if (x<0) x = 0;
    if (y<0) y = 0;
    if (g_pixels)
    {
        int index = x + y * s_width;
        switch (s_bpp)
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

uint32_t sdl_get_pixel(int x, int y)
{
    uint32_t pixel = 0;
    while (x >= s_width) x-= s_width;
    while (y >= s_height) y-= s_height;
    if (x<0) x = 0;
    if (y<0) y = 0;
    if (g_pixels)
    {
        int index = x + y * s_width;
        switch (s_bpp)
        {
            case 8:
                pixel = ((uint8_t *)g_pixels)[ index ];
                break;
            case 16:
                pixel = ((uint16_t *)g_pixels)[ index ];
                break;
            case 32:
                pixel = ((uint32_t *)g_pixels)[ index ];
                break;
            default:
                break;
        }
    }
    return pixel;
}

void sdl_graphics_close(void)
{
    if ( s_unittest_mode )
    {
        return;
    }
    SDL_DestroyWindow(g_window);
}

static uint32_t convert_pixel( uint32_t value, uint8_t target_bpp )
{
    uint32_t pixel;
    switch ( s_pixfmt )
    {
        case SDL_PIXELFORMAT_RGB332: pixel = ((value & 0xE0) << 24) | ((value & 0x1C) << 19) | ((value & 0x03) << 14) | ( 0xFF ); break;
        case SDL_PIXELFORMAT_RGB565: pixel = ((value & 0xF800) << 16) | ((value & 0x07E0) << 13) | ((value & 0x001F) << 11) | ( 0xFF ); break;
        case SDL_PIXELFORMAT_RGBX8888: pixel = value;
        default: pixel = 0; break;
    }
    switch ( target_bpp )
    {
        case 1: pixel = (pixel & 0xFFFFFF00) ? 1 : 0; break;
        case 4: pixel = (pixel & 0x000000F0) >> 4; break;
        case 8: pixel = ((pixel & 0xE0000000) >> 24) | ((pixel & 0x00E00000) >> 19) | ((pixel & 0x0000C000) >> 14); break;
        case 16: pixel = ((pixel & 0xF8000000) >> 16) | ((pixel & 0x00FC0000) >> 13) | ((pixel & 0x0000F800) >> 11); break;
        case 32: break;
        default: break;
    }
    return pixel;
}

void sdl_core_get_pixels_data( uint8_t *pixels, uint8_t target_bpp )
{
    for (int x = 0; x < s_width; x++)
        for (int y = 0; y < s_height; y++)
        {
            uint32_t pixel = sdl_get_pixel( x, y );
            pixel = convert_pixel( pixel, target_bpp );
            switch ( target_bpp )
            {
                case 1: ((uint8_t *)pixels)[x + (y / 8)*s_width] |= (pixel << (y & 0x7));  break;
                case 4: ((uint8_t *)pixels)[x / 2 + y * s_width / 2] |= (pixel << ((x & 1) * 4)); break;
                case 8: ((uint8_t *)pixels)[x + y * s_width] = pixel; break;
                case 16: ((uint16_t *)pixels)[x + y * s_width] = pixel; break;
                case 32: ((uint32_t *)pixels)[x + y * s_width] = pixel; break;
                default: break;
            }
        }
}

int sdl_core_get_pixels_len( uint8_t target_bpp )
{
    int size = s_width * s_height * target_bpp / 8;
    return size;
}

void sdl_core_set_unittest_mode(void)
{
    s_unittest_mode = true;
}
