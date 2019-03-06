/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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
 * @file ssd1306_hal/avr/platform_io.h SSD1306 AVR IO communication functions
 */

#ifndef _SSD1306_AVR_PLATFORM_IO_H_
#define _SSD1306_AVR_PLATFORM_IO_H_

#include "ssd1306_hal/io.h"

#if defined(CONFIG_TWI_I2C_AVAILABLE) && defined(CONFIG_TWI_I2C_ENABLE)

class TwiI2c: public IWireInterface
{
public:
    TwiI2c(uint8_t sa = 0x00);
    virtual ~TwiI2c();

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
    uint8_t m_sa;
};

#endif

#if defined(CONFIG_AVR_SPI_AVAILABLE) && defined(CONFIG_AVR_SPI_ENABLE)

class AvrSpi: public IWireInterface
{
public:
    AvrSpi(int8_t csPin, int8_t dcPin, uint32_t frequency);
    virtual ~AvrSpi();

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
};

#endif

#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)

#elif defined(CONFIG_TWI_I2C_AVAILABLE) && defined(CONFIG_TWI_I2C_ENABLE)

class PlatformI2c: public TwiI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
        TwiI2c(sa) {}
};

#elif defined(CONFIG_SOFTWARE_I2C_AVAILABLE) && defined(CONFIG_SOFTWARE_I2C_ENABLE)

#endif

#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#elif defined(CONFIG_AVR_SPI_AVAILABLE) && defined(CONFIG_AVR_SPI_ENABLE)

class PlatformSpi: public AvrSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t frequency = 8000000):
        AvrSpi(csPin, dcPin, frequency) {}
};

#else

#endif

#endif

