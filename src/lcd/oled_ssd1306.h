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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_INTERFACE_API
 * @{
 */

/**
 * @brief Inits 128x64 OLED display (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display (based on SSD1306 controller).
 * User must init communication interface (i2c, spi) prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ssd1306_128x64_init(void);

/**
 * @brief Inits 128x64 OLED display over i2c (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display over i2c (based on SSD1306 controller)
 * This function uses hardcoded pins for i2c communication, depending on your hardware.
 * If you use non-standard pins in your project, please perform call ssd1306_i2cInitEx() and
 * ssd1306_128x64_init(), or you can use ssd1306_128x64_i2c_initEx().
 */
void         ssd1306_128x64_i2c_init(void);

/**
 * @brief Inits 128x64 OLED display over i2c (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display over i2c (based on SSD1306 controller)
 * This function uses hardcoded pins for i2c communication, depending on your hardware.
 *
 * @param scl - i2c clock pin. Use -1 if you don't need to change default pin number
 * @param sda - i2c data pin. Use -1 if you don't need to change default pin number
 * @param sa  - i2c address of lcd display. Use 0 to leave default
 *
 * @note scl and sda parameters depend on used hardware. For many hardware boards these
 * parameters do not have any effect. ESP8266 allows to specify these parameters
 *
 * @note scl and sda for Linux systems should be the same, and should contain i2c bus id.
 */
void         ssd1306_128x64_i2c_initEx(int8_t scl, int8_t sda, int8_t sa);

/**
 * @brief Inits 128x64 OLED display over spi (based on SSD1306 controller).
 *
 * Inits 128x64 OLED display over spi (based on SSD1306 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1306_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @brief Inits 128x32 OLED display over spi (based on SSD1306 controller).
 *
 * Inits 128x32 OLED display over spi (based on SSD1306 controller)
 * @param rstPin - pin controlling LCD reset (-1 if not used)
 * @param cesPin - chip enable pin to LCD slave (-1 if not used)
 * @param dcPin - data/command pin to control LCD dc (required)
 */
void         ssd1306_128x32_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin);

/**
 * @copydoc ssd1306_128x64_i2c_init
 * @deprecated Use ssd1306_128x64_i2c_init() instead.
 */
void         ssd1306_init(void) __attribute__((deprecated));

/**
 * @brief Inits 128x32 OLED display over i2c (based on SSD1306 controller).
 *
 * Inits 128x32 OLED display over i2c (based on SSD1306 controller)
 * This function uses hardcoded pins for i2c communication, depending on your hardware.
 * If you use non-standard pins in your project, please perform call ssd1306_i2cInitEx() and
 * ssd1306_128x32_init().
 */
void         ssd1306_128x32_i2c_init(void);

/**
 * @brief Inits 128x32 OLED display (based on ssd1306 controller).
 *
 * Inits 128x32 OLED display (based on ssd1306 controller)
 * spi or i2c bus must be initialized prior to calling this function.
 * @see ssd1306_i2cInit()
 * @see ssd1306_spiInit()
 */
void         ssd1306_128x32_init(void);

/**
 * Turns off display
 */
void         ssd1306_displayOff(void);

/**
 * Turns on display
 */
void         ssd1306_displayOn(void);

/**
 * Set display contrast, ie light intensity
 * @param contrast - contrast value to see, refer to ssd1306 datasheet
 */
void         ssd1306_setContrast(uint8_t contrast);

/**
 * Switches display to inverse mode.
 * LCD will display 0-pixels as white, and 1-pixels as black.
 * @note Not supported for SSD1331
 */
void         ssd1306_invertMode(void);

/**
 * Switches display to normal mode.
 * @note Not supported for SSD1331
 */
void         ssd1306_normalMode(void);

/**
 * @brief performs horizontal flip
 *
 * Performs horizontal flip. If you need to turn display by 180 degree,
 * please use both ssd1306_flipHorizontal() and ssd1306_flipVertical().
 *
 * @param mode - 0 to disable horizontal flip
 *               1 to enable horizontal flip
 */
void         ssd1306_flipHorizontal(uint8_t mode);

/**
 * @brief performs vertical flip
 *
 * Performs vertical flip. If you need to turn display by 180 degree,
 * please use both ssd1306_flipHorizontal() and ssd1306_flipVertical().
 *
 * @param mode - 0 to disable vertical flip
 *               1 to enable vertical flip
 */
void         ssd1306_flipVertical(uint8_t mode);

/**
 * Sets start line in gdram to start display content with
 *
 * @param line start line in range 0 - 63
 */
void ssd1306_setStartLine(uint8_t line);

/**
 * returns start line in gdram.
 */
uint8_t ssd1306_getStartLine(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _OLED_SSD1306_H_
