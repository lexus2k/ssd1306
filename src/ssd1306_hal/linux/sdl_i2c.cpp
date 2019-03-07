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
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi_conf.h"
#include "intf/spi/ssd1306_spi.h"

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

//////////////////////////////////////////////////////////////////////////////////
//                        LINUX I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_LINUX_I2C_AVAILABLE) && defined(CONFIG_LINUX_I2C_ENABLE) && \
    defined(SDL_EMULATION)

#include "sdl_core.h"

SdlI2c::SdlI2c(int8_t scl, int8_t sda, uint8_t sa)
{
    sdl_core_init();
}

SdlI2c::~SdlI2c()
{
    sdl_core_close();
}

void SdlI2c::start()
{
    sdl_send_init();
}

void SdlI2c::stop()
{
    sdl_send_stop();
}

void SdlI2c::send(uint8_t data)
{
    sdl_send_byte(data);
}

void SdlI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#endif // CONFIG_PLATFORM_I2C_AVAILABLE

#endif // __linux__
