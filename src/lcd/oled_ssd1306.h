/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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
 * @file oled_ssd1306.h support for OLED ssd1306-based displays
 */


#ifndef _OLED_SSD1306_H_
#define _OLED_SSD1306_H_

#include "ssd1306_hal/io.h"
#include "nano_engine/display.h"
#include "lcd/lcd_common.h"
#include "intf/i2c/ssd1306_i2c.h"

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * Generic interface to ssd1306-based controllers
 */
class DisplaySSD1306: public NanoDisplayOps<1>
{
public:
    /**
     * Created object instance to control ssd1306-based displays
     *
     * @param intf reference to communication interface to use
     * @param dcPin data/command pin, -1 for i2c bus
     */
    DisplaySSD1306(IWireInterface &intf, int8_t dcPin = -1)
        : NanoDisplayOps<1>(intf)
        , m_dc( dcPin ) {}

    /**
     * Switches display to normal mode. This feature is specific
     * for ssd1306 based controllers.
     */
    void normalMode();

    /**
     * Switches display to normal mode. This feature is specific
     * for ssd1306 based controllers.
     */
    void invertMode();

    void setBlock(lcduint_t x, lcduint_t y, lcduint_t w) override;

    void nextPage() override;

    /**
     * Set display contrast, ie light intensity
     * @param contrast - contrast value to see, refer to ssd1306 datasheet
     */
    void setContrast(uint8_t contrast);

    /**
     * Turns off display
     */
    void displayOff();

    /**
     * Turns on display
     */
    void displayOn();

    /**
     * @brief performs horizontal flip
     *
     * Performs horizontal flip. If you need to turn display by 180 degree,
     * please use both flipHorizontal() and flipVertical().
     *
     * @param mode - 0 to disable horizontal flip
     *               1 to enable horizontal flip
     */
    void flipHorizontal(uint8_t mode);

    /**
     * @brief performs vertical flip
     *
     * Performs vertical flip. If you need to turn display by 180 degree,
     * please use both flipHorizontal() and flipVertical().
     *
     * @param mode - 0 to disable vertical flip
     *               1 to enable vertical flip
     */
    void flipVertical(uint8_t mode);

protected:
    int8_t m_dc; ///< data/command pin for SPI, -1 for i2c

    /**
     * Enables either data or command mode on SPI bus
     * @param mode 1 to enable data mode, or 0 to enable command mode
     */
    void spiDataMode(uint8_t mode);

    /**
     * Starts communication with LCD display in command mode.
     * To stop communication use m_intf.end().
     */
    void commandStart();
};

/**
 * Class implements interface to 128x64 ssd1306 monochrome display.
 */
class DisplaySSD1306_128x64: public DisplaySSD1306
{
public:
    using DisplaySSD1306::DisplaySSD1306;

    void begin() override;

    void end() override;
};

/**
 * Class implements interface to 128x32 ssd1306 monochrome display.
 */
class DisplaySSD1306_128x32: public DisplaySSD1306
{
public:
    using DisplaySSD1306::DisplaySSD1306;

    void begin() override;

    void end() override;
};

/**
 * Class implements interface to 128x64 ssd1306 i2c monochrome display.
 */
class DisplaySSD1306_128x64_I2C: public DisplaySSD1306_128x64
{
public:
    /**
     * @brief Inits 128x64 OLED display over i2c (based on SSD1306 controller).
     *
     * Inits 128x64 OLED display over i2c (based on SSD1306 controller)
     * This function uses hardcoded pins for i2c communication, depending on your hardware.
     *
     * @param scl i2c clock pin. Use -1 if you don't need to change default pin number
     * @param sda i2c data pin. Use -1 if you don't need to change default pin number
     * @param sa  i2c address of lcd display. Use 0 to leave default
     *
     * @note scl and sda parameters depend on used hardware. For many hardware boards these
     * parameters do not have any effect. ESP8266 allows to specify these parameters
     *
     * @note scl and sda for Linux systems should be the same, and should contain i2c bus id.
     */
    DisplaySSD1306_128x64_I2C( int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x3C)
        : DisplaySSD1306_128x64(m_i2c, -1)
        , m_i2c( scl, sda, sa ) {}

    void begin() override;

    void end() override;

private:
    PlatformI2c m_i2c;
};

/**
 * Class implements interface to 128x32 ssd1306 i2c monochrome display.
 */
class DisplaySSD1306_128x32_I2C: public DisplaySSD1306_128x32
{
public:
    /**
     * @brief Inits 128x32 OLED display over i2c (based on SSD1306 controller).
     *
     * Inits 128x32 OLED display over i2c (based on SSD1306 controller)
     * This function uses hardcoded pins for i2c communication, depending on your hardware.
     *
     * @param scl i2c clock pin. Use -1 if you don't need to change default pin number
     * @param sda i2c data pin. Use -1 if you don't need to change default pin number
     * @param sa  i2c address of lcd display. Use 0 to leave default
     *
     * @note scl and sda parameters depend on used hardware. For many hardware boards these
     * parameters do not have any effect. ESP8266 allows to specify these parameters
     *
     * @note scl and sda for Linux systems should be the same, and should contain i2c bus id.
     */
    DisplaySSD1306_128x32_I2C( int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x3C)
        : DisplaySSD1306_128x32(m_i2c, -1)
        , m_i2c( scl, sda, sa ) {}

    void begin() override;

    void end() override;

private:
    PlatformI2c m_i2c;
};

/**
 * Class implements interface to 128x64 ssd1306 spi monochrome display.
 */
class DisplaySSD1306_128x64_SPI: public DisplaySSD1306_128x64
{
public:
    /**
     * @brief Inits 128x64 OLED display over spi (based on SSD1306 controller).
     *
     * Inits 128x64 OLED display over spi (based on SSD1306 controller)
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param csPin chip enable pin to LCD slave (-1 if not used)
     * @param dcPin data/command pin to control LCD dc (required)
     */
    DisplaySSD1306_128x64_SPI( int8_t rstPin, int8_t csPin, int8_t dcPin )
        : DisplaySSD1306_128x64( m_spi, dcPin )
        , m_spi( csPin, dcPin, 10000000 ) {}

    void begin() override;

    void end() override;

private:
    PlatformSpi m_spi;
};

/**
 * Class implements interface to 128x32 ssd1306 spi monochrome display.
 */
class DisplaySSD1306_128x32_SPI: public DisplaySSD1306_128x32
{
public:
    /**
     * @brief Inits 128x32 OLED display over spi (based on SSD1306 controller).
     *
     * Inits 128x32 OLED display over spi (based on SSD1306 controller)
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param csPin chip enable pin to LCD slave (-1 if not used)
     * @param dcPin data/command pin to control LCD dc (required)
     */
    DisplaySSD1306_128x32_SPI( int8_t rstPin, int8_t csPin, int8_t dcPin )
        : DisplaySSD1306_128x32( m_spi, dcPin )
        , m_spi( csPin, dcPin, 10000000 ) {}

    void begin() override;

    void end() override;

private:
    PlatformSpi m_spi;
};

/**
 * @}
 */

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1306_H_
