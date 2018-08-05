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

/* Comment out options below if you don't need support in the library,  *
 * and want to reduce memory consumption.                               */

/** Define this macro if you need to enable software I2C module for compilation */
#define CONFIG_SOFTWARE_I2C_ENABLE

/** Define this macro if you need to enable TWI I2C module for compilation */
#define CONFIG_TWI_I2C_ENABLE

/** Define this macro if you need to enable AVR SPI module for compilation */
#define CONFIG_AVR_SPI_ENABLE

/** Define this macro if you need to enable USI SPI module for compilation */
#define CONFIG_USI_SPI_ENABLE

/** Define this macro if you need to enable AVR UART module for compilation */
#define CONFIG_AVR_UART_ENABLE

/** Define this macro if you need to enable VGA module for compilation */
#define CONFIG_VGA_ENABLE

/** Define this macro if you need to enable Adafruit GFX canvas support for compilation */
#ifndef CONFIG_ADAFRUIT_GFX_ENABLE
//#define CONFIG_ADAFRUIT_GFX_ENABLE
#endif

/**
 * Define this macro if platform specific i2c interface is implemented in SSD1306 HAL.
 * If you use Arduino platform, this macro enables Arduino Wire library module for compilation.
 */
#define CONFIG_PLATFORM_I2C_ENABLE

/**
 * Define this macro if platform specific spi interface is implemented in SSD1306 HAL
 * If you use Arduino platform, this macro enables Arduino SPI library module for compilation.
 */
#define CONFIG_PLATFORM_SPI_ENABLE

/**
 * Defines, whenever ssd1306 library supports unicode.
 * Support of unicode increases RAM and Flasg memory consumption
 */
#define CONFIG_SSD1306_UNICODE_ENABLE

/**
 * @}
 */

#endif
