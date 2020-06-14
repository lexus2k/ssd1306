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
#include "intf/ssd1306_interface.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "intf/spi/ssd1306_spi_conf.h"
#include "intf/spi/ssd1306_spi.h"

#ifndef __KERNEL__

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

#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE) \
    && !defined(SDL_EMULATION)
#define LINUX_SPI_AVAILABLE
#endif

#define MAX_GPIO_COUNT   256

#ifdef IN
#undef IN
#endif
#define IN  0

#ifdef OUT
#undef OUT
#endif
#define OUT 1

#ifdef LINUX_SPI_AVAILABLE
static void platform_spi_send_cache();
#endif

int gpio_export(int pin)
{
    char buffer[4];
    ssize_t bytes_written;
    int fd;
    char path[64];

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);

    if (access(path, F_OK) == 0)
    {
        return 0;
    }

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to allocate gpio pin[%d]: %s%s!\n",
                pin, strerror (errno), getuid() == 0 ? "" : ", need to be root");
        return(-1);
    }

    bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
    if (write(fd, buffer, bytes_written) < 0)
    {
        fprintf(stderr, "Failed to allocate gpio pin[%d]: %s%s!\n",
                pin, strerror (errno), getuid() == 0 ? "" : ", need to be root");
        close(fd);
        return -1;
    }
    close(fd);
    return(0);
}

int gpio_unexport(int pin)
{
    char buffer[4];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to free gpio pin resources!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
    if (write(fd, buffer, bytes_written) < 0)
    {
        fprintf(stderr, "Failed to free gpio pin resources!\n");
    }
    close(fd);
    return(0);
}

int gpio_direction(int pin, int dir)
{
    static const char s_directions_str[]  = "in\0out";

    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to set gpio pin direction1[%d]: %s!\n",
                pin, strerror(errno));
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
    {
        fprintf(stderr, "Failed to set gpio pin direction2[%d]: %s!\n",
                pin, strerror(errno));
        return(-1);
    }

    close(fd);
    return(0);
}

int gpio_read(int pin)
{
    char path[32];
    char value_str[3];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to read gpio pin value!\n");
        return(-1);
    }

    if (-1 == read(fd, value_str, 3))
    {
        fprintf(stderr, "Failed to read gpio pin value!\n");
        return(-1);
    }

    close(fd);

    return(atoi(value_str));
}

int gpio_write(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to set gpio pin value[%d]: %s%s!\n",
                pin, strerror (errno), getuid() == 0 ? "" : ", need to be root");
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
    {
        fprintf(stderr, "Failed to set gpio pin value[%d]: %s%s!\n",
                pin, strerror (errno), getuid() == 0 ? "" : ", need to be root");
        return(-1);
    }

    close(fd);
    return(0);
}

#if !defined(SDL_EMULATION)

static uint8_t s_exported_pin[MAX_GPIO_COUNT] = {0};
static uint8_t s_pin_mode[MAX_GPIO_COUNT] = {0};

void pinMode(int pin, int mode)
{
    if (!s_exported_pin[pin])
    {
        if ( gpio_export(pin)<0 )
        {
            return;
        }
        s_exported_pin[pin] = 1;
    }
    if (mode == OUTPUT)
    {
        gpio_direction(pin, OUT);
        s_pin_mode[pin] = 1;
    }
    if (mode == INPUT)
    {
        gpio_direction(pin, IN);
        s_pin_mode[pin] = 0;
    }
}

void digitalWrite(int pin, int level)
{
#ifdef LINUX_SPI_AVAILABLE
    if (s_ssd1306_dc == pin)
    {
        platform_spi_send_cache();
    }
#endif

    if (!s_exported_pin[pin])
    {
        if ( gpio_export(pin)<0 )
        {
            return;
        }
        s_exported_pin[pin] = 1;
    }
    if (!s_pin_mode[pin])
    {
        pinMode(pin, OUTPUT);
    }
    gpio_write( pin, level );
}

#endif // SDL_EMULATION

//////////////////////////////////////////////////////////////////////////////////
//                        LINUX I2C IMPLEMENTATION
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
        fprintf(stderr, "Failed to open the i2c bus %s\n",
                getuid() == 0 ? "": ": need to be root");
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
//                        LINUX SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#if !defined(SDL_EMULATION)

static int     s_spi_fd = -1;
extern uint32_t s_ssd1306_spi_clock;
static uint8_t s_spi_cache[1024];
static int s_spi_cached_count = 0;

static void platform_spi_start(void)
{
    s_spi_cached_count = 0;
}

static void platform_spi_stop(void)
{
    platform_spi_send_cache();
}

static void platform_spi_send_cache()
{
    /* TODO: Yeah, sending single bytes is too slow, but *
     * need to figure out how to detect data/command bytes *
     * to send bytes as one block */
    if ( s_spi_cached_count == 0 )
    {
        return;
    }
    struct spi_ioc_transfer mesg;
    memset(&mesg, 0, sizeof mesg);
    mesg.tx_buf = (unsigned long)&s_spi_cache[0];
    mesg.rx_buf = 0;
    mesg.len = s_spi_cached_count;
    mesg.delay_usecs = 0;
    mesg.speed_hz = 0;
    mesg.bits_per_word = 8;
    mesg.cs_change = 0;
    if (ioctl(s_spi_fd, SPI_IOC_MESSAGE(1), &mesg) < 1)
    {
        fprintf(stderr, "SPI failed to send SPI message: %s\n", strerror (errno)) ;
    }
    s_spi_cached_count = 0;
}

static void platform_spi_send(uint8_t data)
{
    s_spi_cache[s_spi_cached_count] = data;
    s_spi_cached_count++;
    if ( s_spi_cached_count >= sizeof( s_spi_cache ) )
    {
        platform_spi_send_cache();
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
        printf("Failed to initialize SPI: %s%s!\n",
               strerror(errno), getuid() == 0 ? "": ", need to be root");
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

#else  // end of !KERNEL, KERNEL is below

void ssd1306_platform_i2cInit(int8_t busId, uint8_t sa, ssd1306_platform_i2cConfig_t * cfg)
{
}

void ssd1306_platform_spiInit(int8_t busId,
                              int8_t ces,
                              int8_t dcPin)
{
}

#endif // KERNEL

#endif // __linux__
