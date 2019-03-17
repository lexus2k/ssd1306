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

#ifdef __cplusplus

#include "nano_engine/display.h"

class DisplaySSD1331: public NanoDisplayOps<8>
{
public:
    /**
     * Creates instance of SSD1331 controller class
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     * @param dcPin data/command control pin
     */
    DisplaySSD1331(IWireInterface &intf, int8_t rstPin, int8_t dcPin);

    void setBlock(lcduint_t x, lcduint_t y, lcduint_t w) override;

    void nextPage() override;

    void commandStart();

    using NanoDisplayOps<8>::drawLine;

    void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint16_t color);

    /**
     * Copies screen block from one place to another using hardware accelerator
     * features.
     * @param left left position of block to copy
     * @param top top position of block to copy
     * @param right right position of block to copy
     * @param bottom bottom position of block to copy
     * @param newLeft new left position for block being copied
     * @param newTop new top position for block being copied
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
    int8_t m_rstPin;
    int8_t m_dcPin;

    uint8_t m_rotation = 0x00;

    void begin() override;

    /**
     * Enables either data or command mode on SPI bus
     * @param mode 1 to enable data mode, or 0 to enable command mode
     */
    void spiDataMode(uint8_t mode);
};

class DisplaySSD1331_96x64_SPI: public DisplaySSD1331
{
public:
    DisplaySSD1331_96x64_SPI(int8_t rstPin = -1, int8_t csPin = -1,
                             int8_t dcPin = -1, uint32_t freq = 0)
       : DisplaySSD1331(m_spi, rstPin, dcPin)
       , m_spi(csPin, dcPin, freq ? freq : 6000000 ) {}

    void begin();

    void end();

private:
    PlatformSpi m_spi;
};

class DisplaySSD1331x16: public NanoDisplayOps<16>
{
public:
    DisplaySSD1331x16(IWireInterface &intf, int8_t rstPin, int8_t dcPin);

    void setBlock(lcduint_t x, lcduint_t y, lcduint_t w) override;

    void nextPage() override;

    void commandStart();

    using NanoDisplayOps<16>::drawLine;

    void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, uint16_t color);

    /**
     * Copies screen block from one place to another using hardware accelerator
     * features.
     * @param left left position of block to copy
     * @param top top position of block to copy
     * @param right right position of block to copy
     * @param bottom bottom position of block to copy
     * @param newLeft new left position for block being copied
     * @param newTop new top position for block being copied
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

    void spiDataMode(uint8_t mode);
};

class DisplaySSD1331_96x64x16_SPI: public DisplaySSD1331x16
{
public:
    DisplaySSD1331_96x64x16_SPI(int8_t rstPin = -1, int8_t csPin = -1,
                                int8_t dcPin = -1, uint32_t freq = 0)
       : DisplaySSD1331x16(m_spi, rstPin, dcPin)
       , m_spi(csPin, dcPin, freq ? freq : 6000000) {}

    void begin();

    void end();

private:
    PlatformSpi m_spi;
};

extern "C" {
#endif

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * @brief Inits 96x64 RGB OLED display (based on SSD1331 controller).
 *
 * Inits 96x64 RGB OLED display (based on SSD1331 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ssd1331_96x64_init(void);

/**
 * @brief Inits 96x64 RGB OLED display over spi in 8-bit mode (based on SSD1331 controller).
 *
 * Inits 96x64 RGB OLED display over spi in 8-bit mode (based on SSD1331 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1331_96x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @brief Inits 96x64 RGB OLED display over spi in 16-bit mode (based on SSD1331 controller).
 *
 * Inits 96x64 RGB OLED display over spi in 16-bit mode (based on SSD1331 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1331_96x64_spi_init16(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * Draws line
 * @param x1 - x position in pixels of start point
 * @param y1 - y position in pixels of start point
 * @param x2 - x position in pixels of end point
 * @param y2 - y position in pixels of end point
 * @param color - color of the line, refer to RGB_COLOR8 macros
 *
 * @note This API can be used only with ssd1331 RGB oled displays
 */
void         ssd1331_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);

/**
 * Copies block in GDRAM to new position
 * @param left column start of block to copy
 * @param top row start of block to copy
 * @param right column end of block to copy
 * @param bottom row end of block to copy
 * @param newLeft new column start
 * @param newTop new row start
 *
 * @note This API can be used only with ssd1331 RGB oled displays
 * @note after copy command is sent, it takes some time from oled
 *       controller to complete operation. So, it is HIGHLY recommended
 *       to wait for reasonable time before send other graphics operations
 *       (for example, use 250us delay). This time is required for
 *       oled display to become ready to accept new commands.
 */
void ssd1331_copyBlock(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t newLeft, uint8_t newTop);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1331_H_
