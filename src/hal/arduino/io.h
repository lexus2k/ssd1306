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
 * @file hal/arduino/io.h SSD1306 ARDUINO IO communication functions
 */

#ifndef _SSD1306_ARDUINO_IO_H_
#define _SSD1306_ARDUINO_IO_H_

#ifdef ARDUINO

#include <Arduino.h>
#if defined(ESP8266) || defined(ESP32) || defined(ESP31B)
    #include <pgmspace.h>
#else
    #include <avr/pgmspace.h>
    #include <avr/interrupt.h>
    #if !defined(ARDUINO_ARCH_SAMD)
    #include <avr/sleep.h>
    #endif
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

#if defined(ARDUINO_AVR_DIGISPARK) || defined(ARDUINO_AVR_DIGISPARKPRO)
    /** The macro is defined when i2c Wire library is available */
    #define SSD1306_WIRE_SUPPORTED
    #if defined(ARDUINO_AVR_DIGISPARKPRO)
        /** The macro is defined when SPI library is available */
        #define SSD1306_SPI_SUPPORTED
    #else
        /** The macro is defined when software i2c implementation is available */
        #define SSD1306_I2C_SW_SUPPORTED
        #define LCDINT_TYPES_DEFINED
        typedef int8_t lcdint_t;
        typedef uint8_t lcduint_t;
    #endif

#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    /** The macro is defined when software i2c implementation is available */
    #define SSD1306_I2C_SW_SUPPORTED
    /** The macro is defined when USI module is available for use */
    #define SSD1306_USI_SPI_SUPPORTED
    #define LCDINT_TYPES_DEFINED
    typedef int8_t lcdint_t;
    typedef uint8_t lcduint_t;

#elif defined(ESP8266) || defined(ESP32) || defined(ESP31B) || defined(ARDUINO_ARCH_SAMD)
    /* SW implementation of i2c isn't supported on ESP platforms */
    /** The macro is defined when i2c Wire library is available */
    #define SSD1306_WIRE_SUPPORTED
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when SPI library is available */
    #define SSD1306_SPI_SUPPORTED

#else
    /** The macro is defined when i2c Wire library is available */
    #define SSD1306_I2C_SW_SUPPORTED
    /** The macro is defined when i2c Wire library is available */
    #define SSD1306_WIRE_SUPPORTED
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when TWI module is available */
    #define SSD1306_TWI_SUPPORTED
    /** The macro is defined when SPI library is available */
    #define SSD1306_SPI_SUPPORTED
    /** The macro is defined when SPI module is available */
    #define SSD1306_AVR_SPI_SUPPORTED
#endif

/**
 * @}
 */

#ifdef SSD1306_WIRE_SUPPORTED
#ifdef __cplusplus
#include <Wire.h> 
#endif
#endif

#endif /* ARDUINO */

#endif

