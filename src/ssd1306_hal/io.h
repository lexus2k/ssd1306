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
 * @file ssd1306_hal/io.h SSD1306 HAL IO communication functions
 */

#ifndef _SSD1306_HAL_IO_H_
#define _SSD1306_HAL_IO_H_

/**
 * @defgroup SSD1306_HAL_API HAL: ssd1306 library hardware abstraction layer
 * @{
 *
 * @brief i2c/spi ssd1306 library hardware abstraction layer
 *
 * @details ssd1306 library hardware abstraction layer
 */

#include "UserSettings.h"
#include "ssd1306_interface.h"

#if defined(ARDUINO)
#include "arduino/io.h"
#include "arduino/arduino_spi.h"
#include "arduino/arduino_wire.h"
#include "avr/avr_spi.h"
#include "avr/avr_twi.h"
#elif defined(__AVR__) && !defined(ARDUINO)
#include "avr/io.h"
#include "avr/avr_spi.h"
#include "avr/avr_twi.h"
#elif defined(__XTENSA__) && !defined(ARDUINO)
#include "esp/io.h"
#include "esp/esp32_i2c.h"
#include "esp/esp32_spi.h"
#elif defined(STM32F1) || defined(STM32F2) || defined(STM32F4)
#include "stm32/io.h"
#elif defined(__linux__)
#include "linux/io.h"
#include "linux/linux_i2c.h"
#include "linux/linux_spi.h"
#include "linux/sdl_i2c.h"
#include "linux/sdl_spi.h"
#elif defined(__MINGW32__)
#include "mingw/io.h"
#include "linux/sdl_i2c.h"
#include "linux/sdl_spi.h"
#else
#warning "Platform is not supported. Use template to add support"
#include "template/io.h"
#endif

#ifndef LCDINT_TYPES_DEFINED
/** Macro informs if lcdint_t type is defined */
#define LCDINT_TYPES_DEFINED
/** internal int type, used by ssd1306 library. Important for uC with low SRAM */
typedef int lcdint_t;
/** internal int type, used by ssd1306 library. Important for uC with low SRAM */
typedef unsigned int lcduint_t;
#endif

/** swaps content of a and b variables of type type */
#define ssd1306_swap_data(a, b, type)  { type t = a; a = b; b = t; }

#ifdef __cplusplus

////////////////////////////////////////////////////////////////////////////////
//                    I2C PLATFORM
////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_ARDUINO_I2C_AVAILABLE) && defined(CONFIG_ARDUINO_I2C_ENABLE)

class PlatformI2c: public ArduinoI2c
{
public:
     PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
         ArduinoI2c(scl, sda, sa) {}
};

#elif defined(CONFIG_TWI_I2C_AVAILABLE) && defined(CONFIG_TWI_I2C_ENABLE)

class PlatformI2c: public TwiI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
        TwiI2c(sa) {}
};

#elif defined(CONFIG_LINUX_I2C_AVAILABLE) && defined(CONFIG_LINUX_I2C_ENABLE)

#if defined(SDL_EMULATION)
class PlatformI2c: public SdlI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
        SdlI2c(scl, sda, sa) {}
};
#else
class PlatformI2c: public LinuxI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
         LinuxI2c( scl, sa ) {}
};
#endif

#else

#error "Platform not supported"

#endif

////////////////////////////////////////////////////////////////////////////////
//                    SPI PLATFORM
////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_AVR_SPI_AVAILABLE) && defined(CONFIG_AVR_SPI_ENABLE)

class PlatformSpi: public AvrSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t frequency = 8000000):
        AvrSpi(csPin, dcPin, frequency) {}
};

#elif defined(CONFIG_ARDUINO_SPI_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)

class PlatformSpi: public ArduinoSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        ArduinoSpi(csPin, dcPin, freq) {}
};

#elif defined(CONFIG_LINUX_SPI_AVAILABLE) && defined(CONFIG_LINUX_SPI_ENABLE)

#if defined(SDL_EMULATION)
class PlatformSpi: public SdlSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        SdlSpi(dcPin) {}
};
#else
class PlatformSpi: public LinuxSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        LinuxSpi( 0, csPin, dcPin, freq ) {}
};
#endif

#else

#error "Platform not supported"

#endif



extern "C" {
#endif

// !!! PLATFORM I2C IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)
/**
 * @brief Initializes i2c interface for platform being used.
 *
 * Initializes i2c interface for platform being used. i2c implementation
 * depends on platform.
 *
 * @param busId i2c bus number. Some platforms have several i2c buses. so, this
 *        argument identifies bus to use. For AVR platforms busId is used as scl
 *        pin number. If you want to use default i2c bus for specific platform, please
 *        pass -1.
 * @param addr i2c address of oled driver, connected to i2c bus. If you want to use default
 *        i2c display address, please, pass 0.
 * @param arg additional parameter for i2c interface. Not used on many platforms, while
 *        for AVR it is used as sda pin number. If you want to use default pin number, please
 *        pass -1.
 */
void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, int8_t arg);
#endif

// !!! PLATFORM SPI IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)
/**
 * @brief Initializes spi interface for platform being used.
 *
 * Initializes spi interface for platform being used. spi implementation
 * depends on platform.
 *
 * @param busId spi bus number if device has several spi buses. For most AVR platforms
 *        busId is not used. If you want to use default spi bus for specific platform, please
 *        pass -1.
 * @param cesPin chip select (chip enable) pin number. If you want to use default pin,
 *        hard coded by ssd1306 library, please, pass -1. For Linux platform cesPin means
 *        second number in spidev device.
 * @param dcPin data/command pin number. For most oled displays this pin number is used to
 *        select data or command mode for the bus. If you want to use default pin number, please
 *        pass -1.
 */
void ssd1306_platform_spiInit(int8_t busId, int8_t cesPin, int8_t dcPin);
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif


