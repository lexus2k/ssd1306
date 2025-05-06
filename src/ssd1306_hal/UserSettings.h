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
 * @file UserSettings.h SSD1306 modules configuration.
 */

#ifndef _USER_SETTINGS_H_
#define _USER_SETTINGS_H_

/**
 * @defgroup SSD1306_LIBRARY_CONFIG CONFIG: ssd1306 library configuration
 * @{
 *
 * @brief Group of settings allowing to disable/enable library modules
 *
 * @details Arduino IDE (at least as for 1.8.2) has a lack library configuration functionality.
 *          Some Arduino standard libraries are built the way, if you include them to the project,
 *          but do not use any functions from the, they still eat RAM and Flash on you EVK board.
 *          To avoid this, you need competely avoid including of such libraries. SSD1306 library
 *          has a wide interfaces support for different platforms, and even if you don't use
 *          SSD1306 communication via Arduino Wire/Spi/HardwareSerial libraries, those ones do
 *          bad things with your sketch size.
 *          To avoid this you can manually disable SSD1306 modules, you don't need in UserSettings.h
 *          header file, and gain another 100-200 bytes of RAM and 300-500 bytes of Flash.
 */

/* To disable unused library features and save memory,         *
 * define the corresponding SSD1306_xxxxxx_DISABLE options.    */


/** Define SSD1306_SOFTWARE_I2C_DISABLE to exclude software I2C module from compilation. */
#ifndef SSD1306_SOFTWARE_I2C_DISABLE
#define CONFIG_SOFTWARE_I2C_ENABLE
#endif

/** Define SSD1306_TWI_I2C_DISABLE to exclude TWI I2C module from compilation. */
#ifndef SSD1306_TWI_I2C_DISABLE
#define CONFIG_TWI_I2C_ENABLE
#endif

/** Define SSD1306_AVR_SPI_DISABLE to exclude AVR SPI module from compilation. */
#ifndef SSD1306_AVR_SPI_DISABLE
#define CONFIG_AVR_SPI_ENABLE
#endif

/** Define SSD1306_USI_SPI_DISABLE to exclude USI SPI module from compilation. */
#ifndef SSD1306_USI_SPI_DISABLE
#define CONFIG_USI_SPI_ENABLE
#endif

/** Define SSD1306_AVR_UART_DISABLE to exclude AVR UART module from compilation. */
#ifndef SSD1306_AVR_UART_DISABLE
#define CONFIG_AVR_UART_ENABLE
#endif

/** Define SSD1306_VGA_DISABLE to exclude VGA module from compilation. */
#ifndef SSD1306_VGA_DISABLE
#define CONFIG_VGA_ENABLE
#endif

/** Define this macro if you need to enable Adafruit GFX canvas support for compilation */
//#define CONFIG_ADAFRUIT_GFX_ENABLE

/**
 * Define SSD1306_PLATFORM_I2C_DISABLE to disable the platform-specific I2C interface
 * implemented in SSD1306 HAL.
 * 
 * If you use Arduino platform, this lets you skip compilation of the Wire library.
 */
#ifndef SSD1306_PLATFORM_I2C_DISABLE
#define CONFIG_PLATFORM_I2C_ENABLE
#endif

/**
 * Define SSD1306_PLATFORM_SPI_DISABLE to disable the platform-specific SPI interface
 * implemented in SSD1306 HAL.
 * 
 * If you use Arduino platform, this lets you skip compilation of the SPI library.
 */
#ifndef SSD1306_PLATFORM_SPI_DISABLE
#define CONFIG_PLATFORM_SPI_ENABLE
#endif

/**
 * Define SSD1306_UNICODE_DISABLE to disable this library's unicode support.
 * Unicode support increases RAM and Flash memory consumption.
 */
#ifndef SSD1306_UNICODE_DISABLE
#define CONFIG_SSD1306_UNICODE_ENABLE
#endif

/**
 * @}
 */

#endif
