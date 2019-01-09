/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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
 * @file vga_monitor.h Interface to vga_monitor
 */

#ifndef _SSD1306_COMPOSITE_VIDEO_H_
#define _SSD1306_COMPOSITE_VIDEO_H_

#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * @brief Inits 128x64 monochrome VGA display.
 *
 * Inits 128x64 monochrome VGA display. This mode supports 2 colors: black and white.
 * User must init communication interface (uart) for vga client mode or init vga
 * interface for host mode prior to calling this function.
 *
 * @see ssd1306_uartInit_Builtin()
 * @see ssd1306_vga_controller_init()
 */
void composite_video_128x64_mono_init(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

