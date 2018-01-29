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

#if defined(__linux__) && !defined(ARDUINO)

#include "io.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#define IN  0
#define OUT 1
 
int gpio_export(int pin)
{
    char buffer[4];
    ssize_t bytes_written;
    int fd;
 
    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd)
    {
        fprintf(stderr, "Failed to allocate gpio pin resources!\n");
        return(-1);
    }
 
    bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
    write(fd, buffer, bytes_written);
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
    write(fd, buffer, bytes_written);
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
        fprintf(stderr, "Failed to set gpio pin direction!\n");
        return(-1);
    }
 
    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
    {
        fprintf(stderr, "Failed to set gpio pin direction!\n");
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
        fprintf(stderr, "Failed to set gpio pin value!\n");
        return(-1);
    }
 
    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
    {
        fprintf(stderr, "Failed to set gpio pin value!\n");
        return(-1);
    }
 
    close(fd);
    return(0);
}

/* 
    -1 == gpio_export(N)
    -1 == gpio_direction(N, OUT)
    -1 == gpio_write(N, HIGH)
    -1 == gpio_unexport(N)
*/

#endif
