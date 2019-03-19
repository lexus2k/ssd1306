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
/**
 * @file oled_ssd1331.h support for RGB OLED 96x64 display
 */


#ifndef _OLED_SSD1331_H_
#define _OLED_SSD1331_H_

#include "ssd1306_hal/io.h"
#include "lcd/lcd_common.h"
#include "nano_engine/display.h"

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * Class implements basic functions for 8-bit mode of ssd1331-based displays
 */
class DisplaySSD1331: public NanoDisplayOps<8>
{
public:
    /**
     * Creates instance of SSD1331 controller class for 8-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     * @param dcPin data/command control pin
     */
    DisplaySSD1331(IWireInterface &intf, int8_t rstPin, int8_t dcPin);

    void setBlock(lcduint_t x, lcduint_t y, lcduint_t w) override;

    void nextPage() override;

    // Make software implementation of drawLine available for this type of display
    using NanoDisplayOps<8>::drawLine;

    /**
     * Draws line using hardware accelerator capabilities
     *
     * @param x1 x position of first point
     * @param y1 y position of first point
     * @param x2 x position of second point
     * @param y2 y position of second point
     * @param color color to draw line with (refere RGB_COLOR8 macro)
     */
    void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint16_t color);

    /**
     * Copies block in GDRAM to new position using hardware accelerator
     * features.
     * @param left left position of block to copy
     * @param top top position of block to copy
     * @param right right position of block to copy
     * @param bottom bottom position of block to copy
     * @param newLeft new left position for block being copied
     * @param newTop new top position for block being copied
     *
     * @note This API can be used only with ssd1331 RGB oled displays
     * @note after copy command is sent, it takes some time from oled
     *       controller to complete operation. So, it is HIGHLY recommended
     *       to wait for reasonable time before send other graphics operations
     *       (for example, use 250us delay). This time is required for
     *       oled display to become ready to accept new commands.
     */
    void copyBlock(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t newLeft, uint8_t newTop);

    /**
     * @brief Sets screen orientation (rotation)
     *
     * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
     * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
     */
    void setRotation(uint8_t rotation);

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required
    int8_t m_dcPin; ///< indicates data command control pin being used

    uint8_t m_rotation = 0x00;  ///< Indicates display orientation: 0, 1, 2, 3. refer to setRotation

    void begin() override;

    void end() override;

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
 * Class implements ssd1331 96x64 oled display in 8 bit mode over SPI
 */
class DisplaySSD1331_96x64_SPI: public DisplaySSD1331
{
public:
    /**
     * @brief Inits 96x64 OLED display over spi (based on SSD1331 controller): 8-bit mode.
     *
     * Inits 96x64 OLED display over spi (based on SSD1331 controller): 8-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformI2cConfig.
     */
    DisplaySSD1331_96x64_SPI( int8_t rstPin, const SPlatformSpiConfig &config = { -1, -1, -1, 0, -1, -1 } )
        : DisplaySSD1331(m_spi, rstPin, config.dc)
        , m_spi( SPlatformSpiConfig{ config.busId,
                                     config.cs,
                                     config.dc,
                                     config.frequency ?: 6000000,
                                     config.scl,
                                     config.sda } ) {}

    void begin() override;

    void end() override;

private:
    PlatformSpi m_spi;
};

/**
 * Class implements basic functions for 16-bit mode of ssd1331-based displays
 */
class DisplaySSD1331x16: public NanoDisplayOps<16>
{
public:
    /**
     * Creates instance of SSD1331 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     * @param dcPin data/command control pin
     */
    DisplaySSD1331x16(IWireInterface &intf, int8_t rstPin, int8_t dcPin);

    void setBlock(lcduint_t x, lcduint_t y, lcduint_t w) override;

    void nextPage() override;

    // Make software implementation of drawLine available for this type of display
    using NanoDisplayOps<16>::drawLine;

    /**
     * Draws line using hardware accelerator capabilities
     *
     * @param x1 x position of first point
     * @param y1 y position of first point
     * @param x2 x position of second point
     * @param y2 y position of second point
     * @param color color to draw line with (refere RGB_COLOR16 macro)
     */
    void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint16_t color);

    /**
     * Copies block in GDRAM to new position using hardware accelerator
     * features.
     * @param left left position of block to copy
     * @param top top position of block to copy
     * @param right right position of block to copy
     * @param bottom bottom position of block to copy
     * @param newLeft new left position for block being copied
     * @param newTop new top position for block being copied
     *
     * @note This API can be used only with ssd1331 RGB oled displays
     * @note after copy command is sent, it takes some time from oled
     *       controller to complete operation. So, it is HIGHLY recommended
     *       to wait for reasonable time before send other graphics operations
     *       (for example, use 250us delay). This time is required for
     *       oled display to become ready to accept new commands.
     */
    void copyBlock(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t newLeft, uint8_t newTop);

    /**
     * @brief Sets screen orientation (rotation)
     *
     * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
     * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
     */
    void setRotation(uint8_t rotation);

protected:
    int8_t m_rstPin; ///< reset pin number, -1 if reset pin is not used
    int8_t m_dcPin;  ///< data command control pin number

    uint8_t m_rotation = 0x00; ///< indicates display orientation

    void begin() override;

    void end() override;

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
 * Class implements ssd1331 96x64 oled display in 16 bit mode over SPI
 */
class DisplaySSD1331_96x64x16_SPI: public DisplaySSD1331x16
{
public:
    /**
     * @brief Inits 96x64 OLED display over spi (based on SSD1331 controller): 16-bit mode.
     *
     * Inits 96x64 OLED display over spi (based on SSD1331 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformI2cConfig.
     */
    DisplaySSD1331_96x64x16_SPI( int8_t rstPin, const SPlatformSpiConfig &config = { -1, -1, -1, 0, -1, -1 } )
        : DisplaySSD1331x16(m_spi, rstPin, config.dc)
        , m_spi( SPlatformSpiConfig{ config.busId,
                                     config.cs,
                                     config.dc,
                                     config.frequency ?: 6000000,
                                     config.scl,
                                     config.sda } ) {}

    void begin() override;

    void end() override;

private:
    PlatformSpi m_spi;
};

/**
 * @}
 */

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1331_H_
