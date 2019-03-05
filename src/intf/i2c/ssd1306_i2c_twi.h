/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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

/**
 * @file ssd1306_i2c_twi.h SSD1306 i2c communication functions for standard AVR TWI interface
 */

#ifndef _SSD1306_I2C_TWI_H_
#define _SSD1306_I2C_TWI_H_

#include "ssd1306_hal/io.h"
#include "ssd1306_i2c_conf.h"

#if defined(CONFIG_TWI_I2C_AVAILABLE) && defined(CONFIG_TWI_I2C_ENABLE)

#ifdef __cplusplus

class TwiI2c: public IWireInterface
{
public:
    TwiI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00);
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
    int8_t m_scl;
    int8_t m_sda;
    uint8_t m_sa;
};

#if !defined(CONFIG_PLATFORM_I2C_AVAILABLE) || !defined(CONFIG_PLATFORM_I2C_ENABLE)
#define PlatformI2c TwiI2c
#endif

extern "C" {
#endif

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Configures standard TWI AVR module (at 400kHz).
 * This function is called by ssd1306_i2cInit().
 * @param arg - has no meaning for now. Should be zero
 *
 * @note scl and sda pins depend on used hardware.
 */
void ssd1306_i2cConfigure_Twi(uint8_t arg);

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Initializes ssd1306 library to use TWI AVR module for i2c.
 * If you do not know i2c parameters, try ssd1306_i2cInit_Twi(0).
 * SCL and SDA pins depend on platform.
 * @param sa  - i2c address of lcd display. Use 0 to leave default
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void ssd1306_i2cInit_Twi(uint8_t sa);

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_I2C_TWI_H_ */


