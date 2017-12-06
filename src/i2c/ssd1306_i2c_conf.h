/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file ssd1306_i2c_conf.h SSD1306 library basic i2c definitions
 */

#ifndef _SSD1306_I2C_CONF_H_
#define _SSD1306_I2C_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

#ifndef SSD1306_SA
    /**
     * SSD1306_SA defines default i2c address of LCD display. Please, check your device.
     * If you LCD device has different address, you can set different one via
     * ssd1306_i2cInit_Wire() or ssd1306_i2cInit_Embedded() functions.
     * Write command will be SSD1306_SA<<1 and read will be SSD1306_SA<<1 | 1
     */
    #define SSD1306_SA    0x3C  // Slave address
#endif


#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    /**
     * Use embedded i2c on attiny controllers, when Wire library is not applicable.
     * The macro is deprecated. Use SSD1306_I2C_SW_SUPPORTED or SSD1306_WIRE_SUPPORTED instead
     */
    #define SSD1306_EMBEDDED_I2C

    /** The macro is defined when software i2c implementation is available */
    #define SSD1306_I2C_SW_SUPPORTED

    #if defined(ARDUINO_AVR_DIGISPARK)
        /** The macro is defined when i2c Wire library is available */
        #define SSD1306_WIRE_SUPPORTED
    #endif
#else
    /** The macro is defined when i2c Wire library is available */
    #define SSD1306_WIRE_SUPPORTED
    /** The macro is defined when Wire library speed can be configured */
    #if defined(ARDUINO_AVR_DIGISPARKPRO)
        /* Wire.setClock() is not supported by Digispark PRO */
    #else
        #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    #endif
    #if defined(ESP8266) || defined(ESP32) || defined(ESP31B)
        /* SW implementation of i2c isn't supported on ESP platforms */
    #elif defined(ARDUINO_AVR_DIGISPARKPRO)
        /* SW implementation of i2c isn't supported on Digispark PRO */
    #elif defined(ARDUINO_ARCH_SAMD)
        /* SW implementation of i2c isn't supported for SAMD architecture */
    #else
        /** The macro is defined when software i2c implementation is available */
        #define SSD1306_I2C_SW_SUPPORTED
    #endif
#endif

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   3 ///< SCL, Pin 3 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 ///< SDA, Pin 4 on SSD1306 Board
    #endif
#else
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   5 // SCL, Pin A5 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 // SDA, Pin A4 on SSD1306 Board
    #endif
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif