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
 * @file ssd1306_hal/esp/esp32_spi.h This is SPI implementation for ESP32
 */

#ifndef _SSD1306_ESP_ESP32_SPI_H_
#define _SSD1306_ESP_ESP32_SPI_H_

#if defined(CONFIG_ESP32_SPI_AVAILABLE) && defined(CONFIG_ESP32_SPI_ENABLE)

#include "driver/spi_master.h"

/**
 * Class implements spi support for esp32 platform
 */
class EspSpi
{
public:
    /**
     * Creates instance of spi implementation for ESP platform
     * @param busId number of SPI bus to use: HSPI_HOST, VSPI_HOST
     * @param csPin pin number to use as chip select, can be -1
     * @param dcPin pin to use as data command control pin
     * @param frequency frequency in HZ to run spi bus on
     */
    EspSpi( int8_t busId = -1, int8_t csPin = -1, int8_t dcPin = -1,
            uint32_t frequency = 8000000 );
    virtual ~EspSpi();

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
    int8_t m_busId;
    int8_t m_cs;
    int8_t m_dc;
    bool m_first_spi_session;
    uint32_t m_frequency;
    spi_device_handle_t m_spi;
};


#endif

#endif

