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
 * @file hal/arduino/platform_io.h SSD1306 ARDUINO IO communication functions
 */

#ifndef _SSD1306_ARDUINO_PLATFORM_IO_H_
#define _SSD1306_ARDUINO_PLATFORM_IO_H_

#include "io.h"

#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

class ArduinoSpi: public IWireInterface
{
public:
    ArduinoSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0);
    virtual ~ArduinoSpi();

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

class PlatformSpi: public ArduinoSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        ArduinoSpi(csPin, dcPin, freq) {}
};

#endif

#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)
class ArduinoI2c: public IWireInterface
{
public:
    ArduinoI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00);
    virtual ~ArduinoI2c();

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

class PlatformI2c: public ArduinoI2c
{
public:
     PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
         ArduinoI2c(scl, sda, sa) {}
};

#endif

#endif // _SSD1306_ARDUINO_PLATFORM_IO_H_

