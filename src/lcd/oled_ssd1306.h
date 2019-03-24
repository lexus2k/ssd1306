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
#include "display/display.h"
#include "lcd/lcd_common.h"

#include "oled_ssd1306.inl"

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * Class implements interface functions to SSD1306 displays
 */
template <class I>
class InterfaceSSD1306: public I
{
public:
    /**
     * Creates instance of interface to LCD display.
     *
     * @param base Reference to base class, which represents Display
     * @param dc Data/command control pin number, for i2c communication should be -1
     * @param data variable argument list, accepted by platform interface (PlatformI2c, PlatformSpi)
     */
    template <typename... Args>
    InterfaceSSD1306(NanoDisplayBase<InterfaceSSD1306<I>> &base, int8_t dc, Args&&... data)
        : I(data...)
        , m_dc( dc )
        , m_base(base)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For ssd1306 it uses horizontal addressing mode, while for
     * sh1106 the function uses page addressing mode.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - page (top page of the block)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and does not close it.
     *            To close session, please, call endBlock().
     */
    void startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
    {
        commandStart();
        this->send(SSD1306_COLUMNADDR);
        this->send(x);
        this->send(w ? (x + w - 1) : (m_base.width() - 1));
        this->send(SSD1306_PAGEADDR);
        this->send(y);
        this->send((m_base.height() >> 3) - 1);
        if (m_dc >= 0)
        {
            spiDataMode(1);
        }
        else
        {
            this->stop();
            this->start();
            this->send(0x40);
        }
    }

    /**
     * Switches to the start of next RAM page for the block, specified by
     * startBlock().
     * For ssd1306 it does nothing, while for sh1106 the function moves cursor to
     * next page.
     */
    void nextBlock()
    {
    }

    /**
     * Closes data send operation to lcd display.
     */
    void endBlock()
    {
        this->stop();
    }

    /**
     * Enables either data or command mode on SPI bus
     * @param mode 1 to enable data mode, or 0 to enable command mode
     */
    void spiDataMode(uint8_t mode)
    {
        if (m_dc >= 0)
        {
            digitalWrite( m_dc, mode ? HIGH : LOW);
        }
    }

    /**
     * Starts communication with LCD display in command mode.
     * To stop communication use m_intf.end().
     */
    void commandStart()
    {
        this->start();
        if (m_dc >= 0)
            spiDataMode(0);
        else
            this->send(0x00);
    }

private:
    int8_t m_dc = -1; ///< data/command pin for SPI, -1 for i2c
    NanoDisplayBase<InterfaceSSD1306<I>> &m_base; ///< basic lcd display support interface
};

/**
 * Generic interface to ssd1306-based controllers
 */
template <class I>
class DisplaySSD1306: public NanoDisplayOps<NanoDisplayOps1<I>, I>
{
public:
    /**
     * Created object instance to control ssd1306-based displays
     *
     * @param intf reference to communication interface to use
     */
    DisplaySSD1306(I &intf)
        : NanoDisplayOps<NanoDisplayOps1<I>, I>(intf) { }


    /**
     * Switches display to normal mode. This feature is specific
     * for ssd1306 based controllers.
     */
    void normalMode()
    {
        this->m_intf.commandStart();
        this->m_intf.send(SSD1306_NORMALDISPLAY);
        this->m_intf.stop();
    }


    /**
     * Switches display to normal mode. This feature is specific
     * for ssd1306 based controllers.
     */
    void invertMode()
    {
        this->m_intf.commandStart();
        this->m_intf.send(SSD1306_INVERTDISPLAY);
        this->m_intf.stop();
    }

    /**
     * Set display contrast, ie light intensity
     * @param contrast - contrast value to see, refer to ssd1306 datasheet
     */
    void setContrast(uint8_t contrast)
    {
        this->m_intf.commandStart();
        this->m_intf.send(SSD1306_SETCONTRAST);
        this->m_intf.send(contrast);
        this->m_intf.stop();
    }

    /**
     * Turns off display
     */
    void displayOff()
    {
        this->m_intf.commandStart();
        this->m_intf.send(SSD1306_DISPLAYOFF);
        this->m_intf.stop();
    }

    /**
     * Turns on display
     */
    void displayOn()
    {
        this->m_intf.commandStart();
        this->m_intf.send(SSD1306_DISPLAYON);
        this->m_intf.stop();
    }

    /**
     * @brief performs horizontal flip
     *
     * Performs horizontal flip. If you need to turn display by 180 degree,
     * please use both flipHorizontal() and flipVertical().
     *
     * @param mode - 0 to disable horizontal flip
     *               1 to enable horizontal flip
     */
    void flipHorizontal(uint8_t mode)
    {
        this->m_intf.commandStart();
        this->m_intf.send( SSD1306_SEGREMAP | (mode ? 0x00: 0x01 ) );
        this->m_intf.stop();
    }

    /**
     * @brief performs vertical flip
     *
     * Performs vertical flip. If you need to turn display by 180 degree,
     * please use both flipHorizontal() and flipVertical().
     *
     * @param mode - 0 to disable vertical flip
     *               1 to enable vertical flip
     */
    void flipVertical(uint8_t mode)
    {
        this->m_intf.commandStart();
        this->m_intf.send( mode ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC );
        this->m_intf.stop();
    }
};

/**
 * Class implements interface to 128x64 ssd1306 monochrome display.
 */
template <class I>
class DisplaySSD1306_128x64: public DisplaySSD1306<I>
{
public:
    using DisplaySSD1306<I>::DisplaySSD1306;

    /**
     * Initializes 128x64 lcd display. Interface should be ready prior to
     * this function call
     */
    void begin() override
    {
        this->m_w = 128;
        this->m_h = 64;
        for( uint8_t i=0; i < sizeof(s_oled128x64_initData); i++)
        {
            this->m_intf.commandStart();
            this->m_intf.send(pgm_read_byte(&s_oled128x64_initData[i]));
            this->m_intf.stop();
        }
    }

    /**
     * Closes display connection
     */
    void end() override
    {
    }
};

/**
 * Class implements interface to 128x32 ssd1306 monochrome display.
 */
template <class I>
class DisplaySSD1306_128x32: public DisplaySSD1306<I>
{
public:
    using DisplaySSD1306<I>::DisplaySSD1306;

    /**
     * Initializes 128x32 lcd display. Interface should be ready prior to
     * this function call
     */
    void begin() override
    {
        this->m_w = 128;
        this->m_h = 32;
        for( uint8_t i=0; i < sizeof(s_oled128x32_initData); i++)
        {
            this->m_intf.commandStart();
            this->m_intf.send(pgm_read_byte(&s_oled128x32_initData[i]));
            this->m_intf.stop();
        }
    }

    /**
     * Closes display connection
     */
    void end() override
    {
    }
};

/**
 * Class implements interface to 128x64 ssd1306 i2c monochrome display.
 */
class DisplaySSD1306_128x64_I2C: public DisplaySSD1306_128x64<InterfaceSSD1306<PlatformI2c>>
{
public:
    /**
     * @brief Inits 128x64 OLED display over i2c (based on SSD1306 controller).
     *
     * Inits 128x64 OLED display over i2c (based on SSD1306 controller)
     * This function uses hardcoded pins for i2c communication, depending on your hardware.
     *
     * @param config platform i2c configuration. Please refer to SPlatformI2cConfig.
     */
    DisplaySSD1306_128x64_I2C( const SPlatformI2cConfig &config = { -1, 0x3C, -1, -1, 0 } )
        : DisplaySSD1306_128x64(m_i2c)
        , m_i2c( *this, -1,
                 SPlatformI2cConfig{ config.busId,
                                     config.addr ?: (uint8_t)0x3C,
                                     config.scl,
                                     config.sda,
                                     config.frequency } ) {}

    /**
     * Initializes LCD display over I2C.
     */
    void begin() override
    {
        this->m_intf.begin();
        DisplaySSD1306_128x64::begin();
    }

    /**
     * Closes display connection
     */
    void end() override
    {
        DisplaySSD1306_128x64::end();
        this->m_intf.end();
    }

private:
    InterfaceSSD1306<PlatformI2c> m_i2c;
};

/**
 * Class implements interface to 128x32 ssd1306 i2c monochrome display.
 */
class DisplaySSD1306_128x32_I2C: public DisplaySSD1306_128x32<InterfaceSSD1306<PlatformI2c>>
{
public:
    /**
     * @brief Inits 128x32 OLED display over i2c (based on SSD1306 controller).
     *
     * Inits 128x32 OLED display over i2c (based on SSD1306 controller)
     * This function uses hardcoded pins for i2c communication, depending on your hardware.
     *
     * @param config platform i2c configuration. Please refer to SPlatformI2cConfig.
     */
    DisplaySSD1306_128x32_I2C( const SPlatformI2cConfig &config = { -1, 0x3C, -1, -1, 0 } )
        : DisplaySSD1306_128x32(m_i2c)
        , m_i2c( *this, -1,
                 SPlatformI2cConfig{ config.busId,
                                     config.addr ?: (uint8_t)0x3C,
                                     config.scl,
                                     config.sda,
                                     config.frequency } ) {}

    void begin() override
    {
        m_i2c.begin();
        DisplaySSD1306_128x32::begin();
    }

    void end() override
    {
        DisplaySSD1306_128x32::end();
        m_i2c.end();
    }

private:
    InterfaceSSD1306<PlatformI2c> m_i2c;
};

/**
 * Class implements interface to 128x64 ssd1306 spi monochrome display.
 */
class DisplaySSD1306_128x64_SPI: public DisplaySSD1306_128x64<InterfaceSSD1306<PlatformSpi>>
{
public:
    /**
     * @brief Inits 128x64 OLED display over spi (based on SSD1306 controller).
     *
     * Inits 128x64 OLED display over spi (based on SSD1306 controller)
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformI2cConfig.
     */
    DisplaySSD1306_128x64_SPI( int8_t rstPin, const SPlatformSpiConfig &config = { -1, -1, -1, 0, -1, -1 } )
        : DisplaySSD1306_128x64( m_spi )
        , m_spi( *this, config.dc,
                 SPlatformSpiConfig{ config.busId,
                                     config.cs,
                                     config.dc,
                                     config.frequency ?: 10000000,
                                     config.scl,
                                     config.sda } ) {}

    void begin() override
    {
        m_spi.begin();
        DisplaySSD1306_128x64::begin();
    }

    void end() override
    {
        DisplaySSD1306_128x64::end();
        m_spi.end();
    }

private:
    InterfaceSSD1306<PlatformSpi> m_spi;
};

/**
 * Class implements interface to 128x32 ssd1306 spi monochrome display.
 */
class DisplaySSD1306_128x32_SPI: public DisplaySSD1306_128x32<InterfaceSSD1306<PlatformSpi>>
{
public:
    /**
     * @brief Inits 128x32 OLED display over spi (based on SSD1306 controller).
     *
     * Inits 128x32 OLED display over spi (based on SSD1306 controller)
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformI2cConfig.
     */
    DisplaySSD1306_128x32_SPI( int8_t rstPin, const SPlatformSpiConfig &config = { -1, -1, -1, 0, -1, -1 } )
        : DisplaySSD1306_128x32( m_spi )
        , m_spi( *this, config.dc,
                 SPlatformSpiConfig{ config.busId,
                                     config.cs,
                                     config.dc,
                                     config.frequency ?: 10000000,
                                     config.scl,
                                     config.sda } ) {}

    void begin() override
    {
        m_spi.begin();
        DisplaySSD1306_128x32::begin();
    }

    void end() override
    {
        DisplaySSD1306_128x32::end();
        m_spi.end();
    }

private:
    InterfaceSSD1306<PlatformSpi> m_spi;
};

/**
 * @}
 */

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1306_H_
