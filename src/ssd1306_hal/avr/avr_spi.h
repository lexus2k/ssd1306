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
 * @file ssd1306_hal/avr/avr_spi.h SSD1306 AVR SPI communication functions
 */

#ifndef _SSD1306_AVR_SPI_H_
#define _SSD1306_AVR_SPI_H_

#if defined(CONFIG_AVR_SPI_AVAILABLE) && defined(CONFIG_AVR_SPI_ENABLE)

/**
 * Class implements AVR spi support via hardware module.
 */
class AvrSpi: public IWireInterface
{
public:
    /**
     * Creates AVR spi instance. This interface can be used from command line
     * avrgcc toolchain.
     *
     * @param csPin pin to enable chip select signal, or -1 if cs pin is not required.
     * @param dcPin pin to control data/command mode.
     * @param frequency frequency in HZ to run SPI bus.
     */
    AvrSpi(int8_t csPin, int8_t dcPin, uint32_t frequency);

    virtual ~AvrSpi();

    void begin() override;

    void end() override;

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

#endif

#endif

