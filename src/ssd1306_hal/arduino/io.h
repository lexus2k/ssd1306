/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

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

/*
 * @file hal/arduino/io.h SSD1306 ARDUINO IO communication functions
 */

#ifndef _SSD1306_ARDUINO_IO_H_
#define _SSD1306_ARDUINO_IO_H_

#if defined(ARDUINO_ARCH_STM32)   // stm32duino support
    #include <Arduino.h>
    #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32) || defined(ESP31B)  // esp arduino support
    #include <Arduino.h>
    #include <pgmspace.h>
#else  // AVR support
    #include <Arduino.h>
    #include <avr/pgmspace.h>
    #include <avr/interrupt.h>
    #if !defined(ARDUINO_ARCH_SAMD) && defined(__AVR__)
    #include <avr/sleep.h>
    #endif
#endif

#if defined(ARDUINO_ARCH_STM32)
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE
    /** The macro is defined when STM32 i2c implementation is available */
    #define CONFIG_STM32_I2C_AVAILABLE

#elif defined(ARDUINO_AVR_DIGISPARK) || defined(ARDUINO_AVR_DIGISPARKPRO)
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    #if defined(ARDUINO_AVR_DIGISPARKPRO)
        /** The macro is defined when SPI library is available */
        #define CONFIG_PLATFORM_SPI_AVAILABLE
    #else
        /** The macro is defined when software i2c implementation is available */
        #define CONFIG_SOFTWARE_I2C_AVAILABLE
        /* Define lcdint as smallest types to reduce memo usage on tiny controllers. *
         * Remember, that this can cause issues with large lcd displays, i.e. 320x240*/
        #define LCDINT_TYPES_DEFINED
        typedef int8_t lcdint_t;
        typedef uint8_t lcduint_t;
    #endif

#elif defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX4)

    /** The macro is defined when software i2c implementation is available */
    #define CONFIG_SOFTWARE_I2C_AVAILABLE
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when USI module is available for use */
    #define CONFIG_USI_SPI_AVAILABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE
    /** Define lcdint as smallest types to reduce memo usage on tiny controllers. *
     * Remember, that this can cause issues with large lcd displays, i.e. 320x240*/
    #define LCDINT_TYPES_DEFINED
    /** This is for Attiny controllers */
    typedef int8_t lcdint_t;
    /** This is for Attiny controllers */
    typedef uint8_t lcduint_t;
    /** The macro is defined when micro controller doesn't support multiplication operation */
    #define CONFIG_MULTIPLICATION_NOT_SUPPORTED

#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || \
      defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)

    /** The macro is defined when software i2c implementation is available */
    #define CONFIG_SOFTWARE_I2C_AVAILABLE
    /** The macro is defined when USI module is available for use */
    #define CONFIG_USI_SPI_AVAILABLE
    /** Define lcdint as smallest types to reduce memo usage on tiny controllers. *
     * Remember, that this can cause issues with large lcd displays, i.e. 320x240*/
    #define LCDINT_TYPES_DEFINED
    /** This is for Attiny controllers */
    typedef int8_t lcdint_t;
    /** This is for Attiny controllers */
    typedef uint8_t lcduint_t;
    /** The macro is defined when micro controller doesn't support multiplication operation */
    #define CONFIG_MULTIPLICATION_NOT_SUPPORTED

#elif defined(ESP8266) || defined(ESP32) || defined(ESP31B) || \
      defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
    /* SW implementation of i2c isn't supported on ESP platforms */
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE
    #if defined(ESP32)
        /** The macro is defined when composite audio support is available */
        #define CONFIG_VGA_AVAILABLE
    #endif

#elif defined(__AVR_ATmega328P__)
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_SOFTWARE_I2C_AVAILABLE
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when TWI module is available (ATTINY) */
    #define CONFIG_TWI_I2C_AVAILABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE
    /** The macro is defined when SPI module is available (ATMEGA) */
    #define CONFIG_AVR_SPI_AVAILABLE
    /** The macro is defined when UART module is available */
    #define CONFIG_AVR_UART_AVAILABLE
    /** The macro is defined when VGA monitor control is available directly from controller */
    #define CONFIG_VGA_AVAILABLE

#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_SOFTWARE_I2C_AVAILABLE
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when TWI module is available (ATTINY) */
    #define CONFIG_TWI_I2C_AVAILABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE
    // Disable internal AVR SPI implementation for ATMEGA 2560 since,
    // it doesn't work for now
    // #define CONFIG_AVR_SPI_AVAILABLE

#elif defined(NRF52) || defined(NRF5)
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE

#elif defined(__AVR_ATmega4808__) || defined(__AVR_ATmega3208__) || defined(__AVR_ATmega1608__)

    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE

#else
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_SOFTWARE_I2C_AVAILABLE
    /** The macro is defined when i2c Wire library is available */
    #define CONFIG_PLATFORM_I2C_AVAILABLE
    /** The macro is defined when Wire library speed can be configured */
    #define SSD1306_WIRE_CLOCK_CONFIGURABLE
    /** The macro is defined when TWI module is available (ATTINY) */
    #define CONFIG_TWI_I2C_AVAILABLE
    /** The macro is defined when SPI library is available */
    #define CONFIG_PLATFORM_SPI_AVAILABLE
    /** The macro is defined when SPI module is available (ATMEGA) */
    #define CONFIG_AVR_SPI_AVAILABLE
#endif

//#ifdef CONFIG_PLATFORM_I2C_AVAILABLE
//#ifdef __cplusplus
//#include <Wire.h>
//#endif
//#endif

#endif // _SSD1306_ARDUINO_IO_H_

