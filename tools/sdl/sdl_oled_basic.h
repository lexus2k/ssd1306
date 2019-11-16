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

#ifndef _SDL_OLED_BASIC_H_
#define _SDL_OLED_BASIC_H_

#include <stdint.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SSD_COMMAND_NONE   -1

enum
{
    SSD_MODE_NONE,
    SSD_MODE_COMMAND,
    SSD_MODE_DATA,
};

typedef enum
{
    SDMS_AUTO,          // Command arg/write data mode is selected by DC pin or i2c command
    SDMS_CONTROLLER,    // Command arg/write data mode is controlled by LCD controller special command
} sdl_data_mode_selection;

typedef enum
{
    SDM_COMMAND_ARG,    // data mode interprets all data as command args
    SDM_WRITE_DATA,     // data mode sends all data to GDRAM
} sdl_data_mode;

typedef struct
{
    int width;
    int height;
    int bpp;
    uint32_t pixfmt;
    sdl_data_mode_selection dataMode;
    int   (*detect)(uint8_t data);
    void  (*run_cmd)(uint8_t data);
    void  (*run_data)(uint8_t data);
    void  (*reset)(void);
} sdl_oled_info;

#if defined(SDL_NO_BORDER)
const static int BORDER_SIZE = 0;
const static int TOP_HEADER = 0;
const static int RECT_THICKNESS = 0;
#else
const static int BORDER_SIZE = 8;
const static int TOP_HEADER = 16;
const static int RECT_THICKNESS = 2;
#endif
const static int PIXEL_SIZE = 2;

extern int s_commandId;
extern int s_cmdArgIndex;
extern uint8_t s_detected;

extern void sdl_set_data_mode(sdl_data_mode mode);

#ifdef __cplusplus
}
#endif

#endif

