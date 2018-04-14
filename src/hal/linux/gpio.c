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

#if (defined(__linux__) && !defined(__MINGW32__)) && !defined(ARDUINO)

#include "io.h"

#ifndef __KERNEL__

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_GPIO_COUNT   256

#ifdef IN
#undef IN
#endif
#define IN  0

#ifdef OUT
#undef OUT
#endif
#define OUT 1

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
        fprintf(stderr, "Failed to allocate gpio pin resources[%d]: %s!\n", pin, strerror (errno));
        return(-1);
    }

    bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
    if (write(fd, buffer, bytes_written) < 0)
    {
        fprintf(stderr, "Failed to allocate gpio pin resources[%d]: %s!\n", pin, strerror (errno));
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
        fprintf(stderr, "Failed to set gpio pin direction1[%d]: %s!\n", pin, strerror(errno));
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
    {
        fprintf(stderr, "Failed to set gpio pin direction2[%d]: %s!\n", pin, strerror(errno));
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
        fprintf(stderr, "Failed to set gpio pin value[%d]: %s!\n", pin, strerror(errno));
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
    {
        fprintf(stderr, "Failed to set gpio pin value[%d]: %s!\n", pin, strerror (errno));
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

#endif

#endif

#endif
