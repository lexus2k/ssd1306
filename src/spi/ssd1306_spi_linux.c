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

#include "ssd1306_spi_linux.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_spi_conf.h"
#include "ssd1306_spi.h"

#if defined(SSD1306_LINUX_SUPPORTED) && !defined(__KERNEL__)

#include <stdio.h>
#include <unistd.h>

#if !defined(SDL_EMULATION)

/// NO SUPPORT FOR LINUX YET!
void ssd1306_spiInit_Linux(int8_t cesPin, int8_t dcPin)
{
}

#else /* SDL_EMULATION */

#include "sdl_core.h"

void ssd1306_spiInit_Linux(int8_t cesPin, int8_t dcPin)
{
    sdl_core_init();
    ssd1306_startTransmission = sdl_send_init;
    ssd1306_endTransmission = sdl_send_stop;
    ssd1306_sendByte = sdl_send_byte;
    ssd1306_closeInterface = sdl_core_close;
    ssd1306_commandStart = sdl_command_start;
    ssd1306_dataStart = sdl_data_start;
}

#endif /* SDL_EMULATION */

#endif


