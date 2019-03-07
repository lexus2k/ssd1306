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

#if defined(__linux__) && !defined(ARDUINO)

#include "ssd1306_hal/io.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#include <map>

//////////////////////////////////////////////////////////////////////////////////
//                        LINUX SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_LINUX_SPI_AVAILABLE) && defined(CONFIG_LINUX_SPI_ENABLE) && \
    defined(SDL_EMULATION)

#include "sdl_core.h"

SdlSpi::SdlSpi(int8_t dcPin)
{
    sdl_core_init();
    sdl_set_dc_pin(dcPin);
}

SdlSpi::~SdlSpi()
{
    sdl_core_close();
}

void SdlSpi::start()
{
    sdl_send_init();
}

void SdlSpi::stop()
{
    sdl_send_stop();
}

void SdlSpi::send(uint8_t data)
{
    sdl_send_byte(data);
}

void SdlSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#endif /* SDL_EMULATION */

#endif // __linux__
