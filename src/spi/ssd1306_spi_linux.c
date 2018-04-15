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

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static int     s_fd = -1;
extern uint32_t s_ssd1306_spi_clock;

static void ssd1306_spiStart_Linux(void)
{
}

static void ssd1306_spiStop_Linux(void)
{
}

static void ssd1306_spiSendByte_Linux(uint8_t data)
{
    /* TODO: Yeah, sending single bytes is too slow, but   *
     * need to figure out how to detect data/command bytes *
     * to send bytes as one block */
    uint8_t buf[1];
    struct spi_ioc_transfer mesg;
    memset(&mesg, 0, sizeof mesg);
    mesg.tx_buf = (unsigned long)&buf[0],
    mesg.rx_buf = 0,
    mesg.len = 1,
    mesg.delay_usecs = 0,
    mesg.speed_hz = 0,
    mesg.bits_per_word = 8,
    mesg.cs_change = 0,
    buf[0] = data;
    if (ioctl(s_fd, SPI_IOC_MESSAGE(1), &mesg) < 1)
    {
        fprintf(stderr, "SPI failed to send SPI message: %s\n", strerror (errno)) ;
    }
}

static void ssd1306_spiSendBytes_Linux(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        ssd1306_spiSendByte_Linux(*buffer);
        buffer++;
    }
}

static void ssd1306_spiClose_Linux()
{
    if (s_fd >= 0)
    {
        close(s_fd);
        s_fd = -1;
    }
}

static void empty_function(void)
{
}

static void empty_function_arg(uint8_t byte)
{
}

static void empty_function_args(const uint8_t *buffer, uint16_t bytes)
{
}

void ssd1306_spiInit_Linux(int8_t busId, int8_t ces, int8_t dcPin)
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
    ssd1306_intf.start = empty_function;
    ssd1306_intf.stop = empty_function;
    ssd1306_intf.send = empty_function_arg;
    ssd1306_intf.send_buffer = empty_function_args;
    ssd1306_intf.close = empty_function;

    snprintf(filename, 19, "/dev/spidev%d.%d", busId, ces);
    if ((s_fd = open(filename, O_RDWR)) < 0)
    {
        printf("Failed to initialize SPI: %s!\n", strerror(errno));
        return;
    }
    unsigned int speed = s_ssd1306_spi_clock;
    if (ioctl(s_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        printf("Failed to set speed on SPI line: %s!\n", strerror(errno));
    }
    uint8_t mode = SPI_MODE_0;
    if (ioctl (s_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        printf("Failed to set SPI mode: %s!\n", strerror(errno));
    }
    uint8_t spi_bpw = 8;
    if (ioctl (s_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0)
    {
        printf("Failed to set SPI BPW: %s!\n", strerror(errno));
    }

    ssd1306_intf.spi = 1;
    ssd1306_intf.start = ssd1306_spiStart_Linux;
    ssd1306_intf.stop = ssd1306_spiStop_Linux;
    ssd1306_intf.send = ssd1306_spiSendByte_Linux;
    ssd1306_intf.send_buffer = ssd1306_spiSendBytes_Linux;
    ssd1306_intf.close = ssd1306_spiClose_Linux;
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

void ssd1306_spiInit_Linux(int8_t busId, int8_t ces, int8_t dcPin)
{
    sdl_core_init();
    sdl_set_dc_pin(dcPin);
    ssd1306_intf.spi = 1;
    ssd1306_intf.start = sdl_send_init;
    ssd1306_intf.stop = sdl_send_stop;
    ssd1306_intf.send = sdl_send_byte;
    ssd1306_intf.send_buffer = sdl_send_bytes;
    ssd1306_intf.close = sdl_core_close;
}

#endif /* SDL_EMULATION */

#endif

#if defined(__KERNEL__) && defined(SSD1306_LINUX_SUPPORTED)
void ssd1306_spiInit_Linux(int8_t busId, int8_t ces, int8_t dcPin)
{
}
#endif
