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

#include <map>

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

typedef struct
{
    void (*on_pin_change)(void *);
    void *arg;
} SPinEvent;

static uint8_t s_exported_pin[MAX_GPIO_COUNT] = {0};
static uint8_t s_pin_mode[MAX_GPIO_COUNT] = {0};
std::map<int, SPinEvent> s_events;

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
    if (s_events.find(pin) != s_events.end())
    {
        s_events[pin].on_pin_change( s_events[pin].arg );
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

LinuxI2c::LinuxI2c(int8_t busId, uint8_t sa)
    : m_busId( busId )
    , m_sa( sa )
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

LinuxI2c::~LinuxI2c()
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


#else /* SDL_EMULATION */

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

#endif /* SDL_EMULATION */

#endif // CONFIG_PLATFORM_I2C_AVAILABLE


//////////////////////////////////////////////////////////////////////////////////
//                        LINUX SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#if !defined(SDL_EMULATION)

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
    s_events[m_dc].arg = this;
    s_events[m_dc].on_pin_change = OnDcChange;
}

LinuxSpi::~LinuxSpi()
{
    s_events.erase( m_dc );
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

#endif /* SDL_EMULATION */

#endif // CONFIG_PLATFORM_SPI_AVAILABLE

#endif // __linux__
