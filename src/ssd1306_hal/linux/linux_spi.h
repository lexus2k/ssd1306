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
 * @file ssd1306_hal/linux/linux_spi.h SSD1306 LINUX SPI communication functions
 */

#ifndef _SSD1306_LINUX_LINUX_SPI_H_
#define _SSD1306_LINUX_LINUX_SPI_H_

#if defined(CONFIG_LINUX_SPI_AVAILABLE) && defined(CONFIG_LINUX_SPI_ENABLE) && \
    !defined(SDL_EMULATION)

/**
 * Class implements spi bus for linux via spidev interface
 */
class LinuxSpi
{
public:
    /**
     * Creates instance of LinuxSpi class, implementing SPI bus for user-space linux apps
     *
     * @param busId spi bus to use as first number for spidev
     * @param devId spi device number to use as second number for spidev
     * @param dcPin pin to use as data/command mode pin
     * @param frequency frequency to run SPI bus on
     */
    LinuxSpi(int busId, int8_t devId, int8_t dcPin, uint32_t frequency);

    virtual ~LinuxSpi();

    void begin();

    void end();

    /**
     * Starts communication with SSD1306 display.
     */
    void start();

    /**
     * Ends communication with SSD1306 display.
     */
    void stop();

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data);

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size);

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

#endif

#endif

