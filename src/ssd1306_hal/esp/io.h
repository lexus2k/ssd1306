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

/*
 * @file ssd1306_hal/esp/io.h This is stm32 platform file
 */

#ifndef _SSD1306_ESP_IO_H_
#define _SSD1306_ESP_IO_H_

// TODO: To complete support. Any help is welcome

#define SSD1306_ESP_PLATFORM
//========================== I. Include libraries =========================
/* 1. Include all required headers, specific for your platform here */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LOW  0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
/* Progmem attribute for data, located in Flash */
#define PROGMEM

//========================== II. Define options ===========================
/* 2. Uncomment all options, you have support for on your platform   *
 *    Remember that you will need to implement low level intf/i2c or *
 *    intf/spi layers for your platform                              */

/** The macro is defined when STM32 i2c implementation is available */
#define CONFIG_PLATFORM_I2C_AVAILABLE
#define CONFIG_PLATFORM_SPI_AVAILABLE

#ifdef __cplusplus
extern "C" {
#endif

//========================== III. Implement functions =====================
/* Implement functions below the way you like. You can make them non-static */
// !!!  MANDATORY !!!
int  digitalRead(int pin);

void digitalWrite(int pin, int level);

void pinMode(int pin, int mode);

static inline int  analogRead(int pin)    // analogRead()
{
    return 0;
}

uint32_t millis(void);

static inline uint32_t micros(void)       // micros()
{
    return 0;
};

void delay(uint32_t ms);

static inline void delayMicroseconds(uint32_t us)  // delayMicroseconds()
{
}

// !!!  OPTIONAL !!!
static inline void randomSeed(int seed)   // randomSeed() -  can be skipped
{
}

static inline void attachInterrupt(int pin, void (*interrupt)(), int level)  // attachInterrupt() - can be skipped
{
}

static inline uint8_t pgm_read_byte(const void *ptr)  // pgm_read_byte() - can be skipped
{
    return *((const uint8_t *)ptr);
}

static inline uint16_t eeprom_read_word(const void *ptr)  // eeprom_read_word() - can be skipped
{
    return 0;
}

static inline void eeprom_write_word(const void *ptr, uint16_t val) // eeprom_write_word() - can be skipped
{
}

// utoa is already defined in stdlib c
//static inline char *utoa(unsigned int num, char *str, int radix)    // util utoa() - can be skipped
//{
//    *str = '\0';
//    return str;
//}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
static inline int random(int max)   // random(n) - can be skipped if you don't use it
{
    return 0;
}

static inline int random(int min, int max)  // random(a,b) - can be skipped if you don't use it
{
    return 0;
}
#endif

#endif

