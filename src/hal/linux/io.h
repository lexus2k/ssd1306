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
 * @file hal/avr/io.h SSD1306 LINUX IO communication functions
 */

#ifndef _SSD1306_LINUX_IO_H_
#define _SSD1306_LINUX_IO_H_

#if defined(__linux__) && !defined(ARDUINO)

#include <stdint.h>
#include <unistd.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/sleep.h>
//#include <util/delay.h>

#define LOW  0
#define HIGH 1
#define INPUT 1
#define OUTPUT 0
#define PROGMEM

#define SSD1306_LINUX_SUPPORTED
//#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    /** The macro is defined when software i2c implementation is available */
//    #define SSD1306_I2C_SW_SUPPORTED
//#elif defined(__AVR_ATmega328P__)
    /** The macro is defined when software i2c implementation is available */
//    #define SSD1306_I2C_SW_SUPPORTED
    /** The macro is defined when TWI module is available */
//#else
    /** The macro is defined when software i2c implementation is available */
//    #define SSD1306_I2C_SW_SUPPORTED
    /** The macro is defined when TWI module is available */
//    #define SSD1306_TWI_SUPPORTED
//#endif

static inline void digitalWrite(int pin, int level) {};
static inline int  digitalRead(int pin) { return LOW; };
static inline void pinMode(int pin, int mode) {};
static inline void delay(unsigned long ms) { usleep(ms*1000);  };
static inline int  analogRead(int pin) { return 0; };
static inline uint32_t millis() { return 0; };
static inline void randomSeed(int seed) { };
static inline void attachInterrupt(int pin, void (*interrupt)(), int level) { };
static inline uint8_t pgm_read_byte(const void *ptr) { return *((const uint8_t *)ptr); };

#ifdef __cplusplus
static inline int random(int max) { return 0; };
static inline int random(int min, int max) { return 0; };
#endif

#endif

#endif

