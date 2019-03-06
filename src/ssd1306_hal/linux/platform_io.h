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

/*
 * @file ssd1306_hal/linux/platform_io.h SSD1306 LINUX IO communication functions
 */

#ifndef _SSD1306_LINUX_PLATFORM_IO_H_
#define _SSD1306_LINUX_PLATFORM_IO_H_

#include "io.h"

#if defined(SDL_EMULATION)  // SDL Emulation mode includes

class SdlI2c: public IWireInterface
{
public:
    SdlI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00);
    virtual ~SdlI2c();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;
private:
    int8_t m_scl;
    int8_t m_sda;
    uint8_t m_sa;
};

class PlatformI2c: public SdlI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
        SdlI2c(scl, sda, sa) {}
};


class SdlSpi: public IWireInterface
{
public:
    SdlSpi(int8_t dcPin = -1);
    virtual ~SdlSpi();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;
private:
    int8_t m_cs;
    int8_t m_dc;
    uint32_t m_frequency;
};

class PlatformSpi: public SdlSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        SdlSpi(dcPin) {}
};

#else

class LinuxI2c: public IWireInterface
{
public:
    LinuxI2c(int8_t busId = -1, uint8_t sa = 0x00);
    virtual ~LinuxI2c();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;
private:
    int8_t m_busId;
    uint8_t m_sa;
    int m_fd;
    uint16_t m_dataSize;
    uint8_t m_buffer[1024];
};

class PlatformI2c: public LinuxI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
         LinuxI2c( scl, sa ) {}
};


class LinuxSpi: public IWireInterface
{
public:
    LinuxSpi(int busId, int8_t devId, int8_t dcPin, uint32_t frequency);

    virtual ~LinuxSpi();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;

private:
    int m_busId;
    int8_t m_devId;
    int8_t m_dc;
    uint32_t m_frequency;
    uint16_t m_spi_cached_count;
    uint8_t m_spi_cache[1024];
    int m_spi_fd;

    void sendCache();
    static void OnDcChange(void *arg);
};

class PlatformSpi: public LinuxSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        LinuxSpi( 0, csPin, dcPin, freq ) {}
};

#endif

#endif

