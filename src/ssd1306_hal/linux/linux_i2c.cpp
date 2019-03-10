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

#if (defined(__linux__) || defined(__MINGW32__)) && !defined(ARDUINO)

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

//////////////////////////////////////////////////////////////////////////////////
//                        LINUX I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_LINUX_I2C_AVAILABLE) && defined(CONFIG_LINUX_I2C_ENABLE) && \
    !defined(SDL_EMULATION)

LinuxI2c::LinuxI2c(int8_t busId, uint8_t sa)
    : m_busId( busId )
    , m_sa( sa )
{
}

LinuxI2c::~LinuxI2c()
{
    if (m_fd >= 0)
    {
        close(m_fd);
        m_fd = -1;
    }
}

void LinuxI2c::begin()
{
    char filename[20];
    if (m_busId < 0)
    {
        m_busId = 1;
    }
    snprintf(filename, 19, "/dev/i2c-%d", m_busId);
    if ((m_fd = open(filename, O_RDWR)) < 0)
    {
        fprintf(stderr, "Failed to open the i2c bus %s\n",
                getuid() == 0 ? "": ": need to be root");
        return;
    }
    if (ioctl(m_fd, I2C_SLAVE, m_sa) < 0)
    {
        fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
        return;
    }
}

void LinuxI2c::end()
{
    if (m_fd >= 0)
    {
        close(m_fd);
        m_fd = -1;
    }
}

void LinuxI2c::start()
{
    m_dataSize = 0;
}

void LinuxI2c::stop()
{
    if (write(m_fd, m_buffer, m_dataSize) != m_dataSize)
    {
        fprintf(stderr, "Failed to write to the i2c bus: %s.\n", strerror(errno));
    }
    m_dataSize = 0;
}

void LinuxI2c::send(uint8_t data)
{
    m_buffer[m_dataSize] = data;
    m_dataSize++;
    if (m_dataSize == sizeof(m_buffer))
    {
        /* Send function puts all data to internal buffer.  *
         * Restart transmission if internal buffer is full. */
        stop();
        start();
        send(0x40);
    }
}

void LinuxI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#endif // CONFIG_PLATFORM_I2C_AVAILABLE

#endif // __linux__
