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
#include "sdl_graphics.h"
#include "sdl_oled_basic.h"
#include "sdl_ssd1306.h"
#include "sdl_ssd1325.h"
#include "sdl_ssd1331.h"
#include "sdl_ssd1351.h"
#include "sdl_il9163.h"
#include "sdl_ili9341.h"
#include "sdl_pcd8544.h"
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CANVAS_REFRESH_RATE  60

enum
{
    SDL_AUTODETECT,
    SDL_DETECTED,
};

static int s_analogInput[128];
static int s_digitalPins[128];
static int s_dcPin = -1;
static uint8_t s_gpioKeys[6] = {0};

static sdl_oled_info *p_oled_db[128] = { NULL };
static sdl_oled_info *p_active_driver = NULL;

int s_commandId = SSD_COMMAND_NONE;
int s_cmdArgIndex;
static int s_ssdMode = SSD_MODE_NONE;
static sdl_data_mode s_active_data_mode = SDM_COMMAND_ARG;

static int s_oled = SDL_AUTODETECT;


static void register_oled(sdl_oled_info *oled_info)
{
    sdl_oled_info **p = p_oled_db;
    while (*p) (p++);
    *p = oled_info;
    if ( oled_info->reset )
    {
        oled_info->reset();
    }
}

static void unregister_oleds(void)
{
    memset( p_oled_db, 0, sizeof( p_oled_db ) );
    p_active_driver = NULL;
}

void sdl_core_init(void)
{
    s_commandId = SSD_COMMAND_NONE;
    s_ssdMode = SSD_MODE_NONE;
    s_active_data_mode = SDM_COMMAND_ARG;
    s_oled = SDL_AUTODETECT;
    s_dcPin = -1;
    memset(s_gpioKeys, 0, sizeof(s_gpioKeys));

    register_oled( &sdl_ssd1306 );
    register_oled( &sdl_ssd1325 );
    register_oled( &sdl_ssd1331x8 );
    register_oled( &sdl_ssd1331x16 );
    register_oled( &sdl_ssd1351 );
    register_oled( &sdl_il9163 );
    register_oled( &sdl_ili9341 );
    register_oled( &sdl_pcd8544 );
    sdl_graphics_init();
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
                if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)  { s_analogInput[0] = 300; s_digitalPins[s_gpioKeys[0]] = 1; }
                if (event.key.keysym.scancode == SDL_SCANCODE_UP)    { s_analogInput[0] = 150; s_digitalPins[s_gpioKeys[3]] = 1; }
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)  { s_analogInput[0] = 500; s_digitalPins[s_gpioKeys[1]] = 1; }
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) { s_analogInput[0] = 50;  s_digitalPins[s_gpioKeys[2]] = 1; }
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) { s_analogInput[0] = 700; s_digitalPins[s_gpioKeys[4]] = 1; }
                if (event.key.keysym.scancode == SDL_SCANCODE_Z) { s_analogInput[0] = 700; s_digitalPins[s_gpioKeys[4]] = 1; }
                if (event.key.keysym.scancode == SDL_SCANCODE_X) { s_digitalPins[s_gpioKeys[5]] = 1; }
                break;
            case SDL_KEYUP:
                if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)  { s_analogInput[0] = 1023; s_digitalPins[s_gpioKeys[0]] = 0; }
                if (event.key.keysym.scancode == SDL_SCANCODE_UP)    { s_analogInput[0] = 1023; s_digitalPins[s_gpioKeys[3]] = 0; }
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)  { s_analogInput[0] = 1023; s_digitalPins[s_gpioKeys[1]] = 0; }
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) { s_analogInput[0] = 1023;  s_digitalPins[s_gpioKeys[2]] = 0; }
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) { s_analogInput[0] = 1023; s_digitalPins[s_gpioKeys[4]] = 0; }
                if (event.key.keysym.scancode == SDL_SCANCODE_Z) { s_analogInput[0] = 1023; s_digitalPins[s_gpioKeys[4]] = 0; }
                if (event.key.keysym.scancode == SDL_SCANCODE_X) { s_digitalPins[s_gpioKeys[5]] = 0; }
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

void sdl_set_gpio_keys(const uint8_t * pins)
{
    memcpy(s_gpioKeys, pins, sizeof(s_gpioKeys));
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

int sdl_read_digital(int pin)
{
    return s_digitalPins[pin];
}

void sdl_core_close(void)
{
    sdl_graphics_close();
    SDL_Quit();
    unregister_oleds();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void sdl_send_init()
{
    s_active_data_mode = SDM_COMMAND_ARG;
    s_ssdMode = SSD_MODE_NONE;
//    s_commandId = SSD_COMMAND_NONE;
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
                    s_commandId = SSD_COMMAND_NONE;
                    sdl_graphics_set_oled_params(
                        p_active_driver->width,
                        p_active_driver->height,
                        p_active_driver->bpp,
                        p_active_driver->pixfmt);
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
    sdl_poll_event();
    sdl_graphics_refresh();
    s_ssdMode = -1;
}

void sdl_set_data_mode(sdl_data_mode mode)
{
    s_active_data_mode = mode;
}
