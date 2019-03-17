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
 * @file ssd1306_hal/linux/linux_i2c.h SSD1306 LINUX I2C communication functions
 */

#ifndef _SSD1306_LINUX_LINUX_I2C_H_
#define _SSD1306_LINUX_LINUX_I2C_H_

#if defined(CONFIG_LINUX_I2C_AVAILABLE) && defined(CONFIG_LINUX_I2C_ENABLE) && \
    !defined(SDL_EMULATION)

/**
 * Class implements i2c interface for Linux via i2c-dev
 */
class LinuxI2c: public IWireInterface
{
public:
    /**
     * Creates instance of I2C implementation for Linux (via i2cdev)
     *
     * @param busId i2c bus number, if -1 defaults to 1
     * @param sa i2c address of the display (7 bits)
     */
    LinuxI2c(int8_t busId = -1, uint8_t sa = 0x00);
    virtual ~LinuxI2c();

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
    int8_t m_busId;
    uint8_t m_sa;
    int m_fd;
    uint16_t m_dataSize;
    uint8_t m_buffer[1024];
};

#endif

#endif

