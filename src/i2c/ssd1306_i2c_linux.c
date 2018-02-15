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

#include "ssd1306_i2c_linux.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_i2c_conf.h"
#include "ssd1306_i2c.h"

#if defined(SSD1306_LINUX_SUPPORTED) && !defined(__KERNEL__)

#include <stdio.h>
#include <unistd.h>

#if !defined(SDL_EMULATION)

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

static uint8_t s_sa = SSD1306_SA;
static int     s_fd = -1;
static uint8_t s_buffer[128];
static uint8_t s_dataSize = 0;

static void ssd1306_i2cStart_Linux(void)
{
    s_dataSize = 0;
}

static void ssd1306_i2cStop_Linux(void)
{
    if (write(s_fd, s_buffer, s_dataSize) != s_dataSize)
    {
        fprintf(stderr, "Failed to write to the i2c bus.\n");
    }
    s_dataSize = 0;
}

static void ssd1306_i2cSendByte_Linux(uint8_t data)
{
    s_buffer[s_dataSize] = data;
    s_dataSize++;
    if (s_dataSize == sizeof(s_buffer))
    {
        /* Send function puts all data to internal buffer.  *
         * Restart transmission if internal buffer is full. */
        ssd1306_i2cStop_Linux();
        ssd1306_dataStart();
    }
}

static void ssd1306_i2cClose_Linux()
{
    if (s_fd >= 0)
    {
        close(s_fd);
        s_fd = -1;
    }
}

static void empty_function()
{
}

static void empty_function_single_arg(uint8_t arg)
{
}

void ssd1306_i2cInit_Linux(int8_t busId, uint8_t sa)
{
    char filename[20];
    if (busId < 0)
    {
        busId = 1;
    }
    snprintf(filename, 19, "/dev/i2c-%d", busId);
    ssd1306_startTransmission = empty_function;
    ssd1306_endTransmission = empty_function;
    ssd1306_closeInterface = empty_function;
    ssd1306_commandStart = empty_function;
    ssd1306_dataStart = empty_function;
    ssd1306_sendByte = empty_function_single_arg;
    if ((s_fd = open(filename, O_RDWR)) < 0)
    {
        fprintf(stderr, "Failed to open the i2c bus\n");
        return;
    }
    if (sa)
    {
        s_sa = sa;
    }
    if (ioctl(s_fd, I2C_SLAVE, s_sa) < 0)
    {
        fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
        return;
    }
    ssd1306_startTransmission = ssd1306_i2cStart_Linux;
    ssd1306_endTransmission = ssd1306_i2cStop_Linux;
    ssd1306_sendByte = ssd1306_i2cSendByte_Linux;
    ssd1306_closeInterface = ssd1306_i2cClose_Linux;
    ssd1306_commandStart = ssd1306_i2cCommandStart;
    ssd1306_dataStart = ssd1306_i2cDataStart;
}

#else /* SDL_EMULATION */

#include "sdl_core.h"

void ssd1306_i2cInit_Linux(int8_t busId, uint8_t sa)
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


