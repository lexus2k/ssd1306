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

#if defined(__MINGW32__)

#include "ssd1306_hal/io.h"

#include "intf/ssd1306_interface.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi_conf.h"
#include "intf/spi/ssd1306_spi.h"

#include <stdio.h>
#include <unistd.h>

#if !defined(SDL_EMULATION)

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <linux/spi/spidev.h>

#endif

//////////////////////////////////////////////////////////////////////////////////
//                        MINGW I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)

#if !defined(SDL_EMULATION)

static uint8_t s_sa = SSD1306_SA;
static int     s_fd = -1;
static uint8_t s_buffer[128];
static uint8_t s_dataSize = 0;

static void platform_i2c_start(void)
{
    s_dataSize = 0;
}

static void platform_i2c_stop(void)
{
    if (write(s_fd, s_buffer, s_dataSize) != s_dataSize)
    {
        fprintf(stderr, "Failed to write to the i2c bus: %s.\n", strerror(errno));
    }
    s_dataSize = 0;
}

static void platform_i2c_send(uint8_t data)
{
    s_buffer[s_dataSize] = data;
    s_dataSize++;
    if (s_dataSize == sizeof(s_buffer))
    {
        /* Send function puts all data to internal buffer.  *
         * Restart transmission if internal buffer is full. */
        ssd1306_intf.stop();
        ssd1306_intf.start();
        ssd1306_intf.send(0x40);
    }
}

static void platform_i2c_send_buffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        platform_i2c_send(*buffer);
        buffer++;
    }
}

static void platform_i2c_close()
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

static void empty_function_two_args(const uint8_t *arg1, uint16_t arg2)
{
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t sa, ssd1306_platform_i2cConfig_t * cfg)
{
    char filename[20];
    if (busId < 0)
    {
        busId = 1;
    }
    snprintf(filename, 19, "/dev/i2c-%d", busId);
    ssd1306_intf.start = empty_function;
    ssd1306_intf.stop = empty_function;
    ssd1306_intf.close = empty_function;
    ssd1306_intf.send = empty_function_single_arg;
    ssd1306_intf.send_buffer = empty_function_two_args;
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
    ssd1306_intf.start = platform_i2c_start;
    ssd1306_intf.stop = platform_i2c_stop;
    ssd1306_intf.send = platform_i2c_send;
    ssd1306_intf.send_buffer = platform_i2c_send_buffer;
    ssd1306_intf.close = platform_i2c_close;
}

#else /* SDL_EMULATION */

#include "sdl_core.h"

static void platform_i2c_send_buffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        sdl_send_byte(*buffer);
        buffer++;
    };
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t sa, ssd1306_platform_i2cConfig_t * cfg)
{
    sdl_core_init();
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = sdl_send_init;
    ssd1306_intf.stop = sdl_send_stop;
    ssd1306_intf.send = sdl_send_byte;
    ssd1306_intf.send_buffer = platform_i2c_send_buffer;
    ssd1306_intf.close = sdl_core_close;
}

#endif /* SDL_EMULATION */

#endif // CONFIG_PLATFORM_I2C_AVAILABLE


//////////////////////////////////////////////////////////////////////////////////
//                        MINGW SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#if !defined(SDL_EMULATION)

static int     s_spi_fd = -1;
extern uint32_t s_ssd1306_spi_clock;

static void platform_spi_start(void)
{
}

static void platform_spi_stop(void)
{
}

static void platform_spi_send(uint8_t data)
{
    /* TODO: Yeah, sending single bytes is too slow, but *
     * need to figure out how to detect data/command bytes *
     * to send bytes as one block */
    uint8_t buf[1];
    struct spi_ioc_transfer mesg;
    buf[0] = data;
    memset(&mesg, 0, sizeof mesg);
    mesg.tx_buf = (unsigned long)&buf[0];
    mesg.rx_buf = 0;
    mesg.len = 1;
    mesg.delay_usecs = 0;
    mesg.speed_hz = 0;
    mesg.bits_per_word = 8;
    mesg.cs_change = 0;
    if (ioctl(s_spi_fd, SPI_IOC_MESSAGE(1), &mesg) < 1)
    {
        fprintf(stderr, "SPI failed to send SPI message: %s\n", strerror (errno)) ;
    }
}

static void platform_spi_close(void)
{
    if (s_spi_fd >= 0)
    {
        close(s_spi_fd);
        s_spi_fd = -1;
    }
}

static void platform_spi_send_buffer(const uint8_t *data, uint16_t len)
{
    while (len--)
    {
        platform_spi_send(*data);
        data++;
    }
}

static void empty_function_spi(void)
{
}

static void empty_function_arg_spi(uint8_t byte)
{
}

static void empty_function_args_spi(const uint8_t *buffer, uint16_t bytes)
{
}

void ssd1306_platform_spiInit(int8_t busId,
                              int8_t ces,
                              int8_t dcPin)
{
    char filename[20];
    if (busId < 0)
    {
        busId = 0;
    }
    if (ces < 0)
    {
        ces = 0;
    }
    s_ssd1306_cs = -1;    // SPI interface does't need separate ces pin
    s_ssd1306_dc = dcPin;
    ssd1306_intf.spi = 1;
    ssd1306_intf.start = empty_function_spi;
    ssd1306_intf.stop = empty_function_spi;
    ssd1306_intf.send = empty_function_arg_spi;
    ssd1306_intf.send_buffer = empty_function_args_spi;
    ssd1306_intf.close = empty_function;

    snprintf(filename, 19, "/dev/spidev%d.%d", busId, ces);
    if ((s_spi_fd = open(filename, O_RDWR)) < 0)
    {
        printf("Failed to initialize SPI: %s!\n", strerror(errno));
        return;
    }
    unsigned int speed = s_ssd1306_spi_clock;
    if (ioctl(s_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        printf("Failed to set speed on SPI line: %s!\n", strerror(errno));
    }
    uint8_t mode = SPI_MODE_0;
    if (ioctl (s_spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        printf("Failed to set SPI mode: %s!\n", strerror(errno));
    }
    uint8_t spi_bpw = 8;
    if (ioctl (s_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0)
    {
        printf("Failed to set SPI BPW: %s!\n", strerror(errno));
    }

    ssd1306_intf.spi = 1;
    ssd1306_intf.start = platform_spi_start;
    ssd1306_intf.stop = platform_spi_stop;
    ssd1306_intf.send = platform_spi_send;
    ssd1306_intf.send_buffer = platform_spi_send_buffer;
    ssd1306_intf.close = platform_spi_close;
}

#else /* SDL_EMULATION */

#include "sdl_core.h"

static void sdl_send_bytes(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        sdl_send_byte(*buffer);
        buffer++;
    };
}

void ssd1306_platform_spiInit(int8_t busId, int8_t ces, int8_t dcPin)
{
    sdl_core_init();
    if (ces >= 0)
    {
        s_ssd1306_cs = ces;
    }
    if (dcPin >= 0)
    {
        s_ssd1306_dc = dcPin;
    }
    sdl_set_dc_pin(dcPin);
    ssd1306_intf.spi = 1;
    ssd1306_intf.start = sdl_send_init;
    ssd1306_intf.stop = sdl_send_stop;
    ssd1306_intf.send = sdl_send_byte;
    ssd1306_intf.send_buffer = sdl_send_bytes;
    ssd1306_intf.close = sdl_core_close;
}

#endif /* SDL_EMULATION */

#endif // CONFIG_PLATFORM_SPI_AVAILABLE

#endif // __MINGW32__

