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
//                        LINUX SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_LINUX_SPI_AVAILABLE) && defined(CONFIG_LINUX_SPI_ENABLE) && \
    !defined(SDL_EMULATION)

LinuxSpi::LinuxSpi(int busId, int8_t devId, int8_t dcPin, uint32_t frequency)
    : m_busId( busId )
    , m_devId( devId )
    , m_dc( dcPin )
    , m_frequency( frequency )
    , m_spi_cached_count( 0 )
{
    char filename[20];
    if (m_busId < 0)
    {
        m_busId = 0; // SPI bus - default 0
    }
    if (m_devId < 0)
    {
        m_devId = 0; // SPI device - default 0
    }

    snprintf(filename, 19, "/dev/spidev%d.%d", m_busId, m_devId);
    if ((m_spi_fd = open(filename, O_RDWR)) < 0)
    {
        printf("Failed to initialize SPI: %s%s!\n",
               strerror(errno), getuid() == 0 ? "": ", need to be root");
        return;
    }
    unsigned int speed = m_frequency;
    if (ioctl(m_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        printf("Failed to set speed on SPI line: %s!\n", strerror(errno));
    }
    uint8_t mode = SPI_MODE_0;
    if (ioctl (m_spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        printf("Failed to set SPI mode: %s!\n", strerror(errno));
    }
    uint8_t spi_bpw = 8;
    if (ioctl (m_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0)
    {
        printf("Failed to set SPI BPW: %s!\n", strerror(errno));
    }
    // THIS IS HACK TO GET NOTIFICATIONS ON DC PIN CHANGE
    ssd1306_registerPinEvent(m_dc, OnDcChange, this);
}

LinuxSpi::~LinuxSpi()
{
    ssd1306_unregisterPinEvent( m_dc );
    if (m_spi_fd >= 0)
    {
        close(m_spi_fd);
        m_spi_fd = -1;
    }
}

void LinuxSpi::start()
{
    m_spi_cached_count = 0;
}

void LinuxSpi::stop()
{
    sendCache();
}

void LinuxSpi::OnDcChange(void *arg)
{
    LinuxSpi *obj = reinterpret_cast<LinuxSpi*>(arg);
    obj->sendCache();
}

void LinuxSpi::sendCache()
{
    /* TODO: Yeah, sending single bytes is too slow, but *
     * need to figure out how to detect data/command bytes *
     * to send bytes as one block */
    if ( m_spi_cached_count == 0 )
    {
        return;
    }
    struct spi_ioc_transfer mesg;
    memset(&mesg, 0, sizeof mesg);
    mesg.tx_buf = (unsigned long)&m_spi_cache[0];
    mesg.rx_buf = 0;
    mesg.len = m_spi_cached_count;
    mesg.delay_usecs = 0;
    mesg.speed_hz = 0;
    mesg.bits_per_word = 8;
    mesg.cs_change = 0;
    if (ioctl(m_spi_fd, SPI_IOC_MESSAGE(1), &mesg) < 1)
    {
        fprintf(stderr, "SPI failed to send SPI message: %s\n", strerror (errno)) ;
    }
    m_spi_cached_count = 0;
}

void LinuxSpi::send(uint8_t data)
{
    m_spi_cache[m_spi_cached_count] = data;
    m_spi_cached_count++;
    if ( m_spi_cached_count >= sizeof( m_spi_cache ) )
    {
        sendCache();
    }
}

void LinuxSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#else /* SDL_EMULATION */

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

#endif // CONFIG_PLATFORM_SPI_AVAILABLE

#endif // __linux__
