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
/**
 * @file vga_controller_base.h VGA basic data. Do not include this header in your project
 */

#ifndef _SSD1306_VGA_CONTROLLER_BASE_H_
#define _SSD1306_VGA_CONTROLLER_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ssd1306_hal/io.h"

static const uint8_t H_SYNC_PIN = 3;
static const uint8_t V_SYNC_PIN = 10;

extern uint8_t s_vga_buffer[];

void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t));

void ssd1306_vgaController_init_no_output(void);

#if defined(SSD1306_BUILTIN_VGA_SUPPORT)
void ssd1306_vgaController_init_enable_output(void);
#endif

static inline void ssd1306_vgaController_init(void)
{
    // if there is no builtin support then only debug mode is available
#if defined(VGA_CONTROLLER_DEBUG) || !defined(SSD1306_BUILTIN_VGA_SUPPORT)
    ssd1306_vgaController_init_no_output();
#else
    ssd1306_vgaController_init_enable_output();
#endif
}

#ifdef __cplusplus
}
#endif

#endif

