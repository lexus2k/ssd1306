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
 * @file ssd1306_hal/mingw/io.h SSD1306 MINGW IO communication functions
 */

#ifndef _SSD1306_MINGW_IO_H_
#define _SSD1306_MINGW_IO_H_

// Use the same library interface as for Linux
#define CONFIG_PLATFORM_I2C_AVAILABLE
#define CONFIG_PLATFORM_SPI_AVAILABLE

#if defined(SDL_EMULATION)  // SDL Emulation mode includes
#include "sdl_core.h"
#endif

#include <windows.h>
#include <stdint.h>
#include <unistd.h>
#include "sdl_core.h"
#include <time.h>
#include <string.h>

/** Standard defines */
#define LOW  0
#define HIGH 1
#define INPUT 1
#define OUTPUT 0
/** Pure linux implementation of the library doesn't support data, located in code area */
#define PROGMEM

#ifdef __cplusplus
extern "C" {
#endif

static inline int  digitalRead(int pin) { return sdl_read_digital(pin); };
static inline void delay(unsigned long ms) { Sleep(ms);  };
static inline void delayMicroseconds(unsigned long us) { Sleep((us+500)/1000); };
static inline uint32_t millis(void)
{
    return GetTickCount();
};

static inline uint32_t micros(void)
{
    return GetTickCount()*1000;
};

#if defined(SDL_EMULATION)
static inline int  analogRead(int pin) { return sdl_read_analog(pin); };
static inline void digitalWrite(int pin, int level) {  sdl_write_digital(pin, level); };
static inline void pinMode(int pin, int mode) { };
#endif

static inline void randomSeed(int seed) { };
static inline void attachInterrupt(int pin, void (*interrupt)(void), int level) { };
static inline uint8_t pgm_read_byte(const void *ptr) { return *((const uint8_t *)ptr); };
static inline uint16_t eeprom_read_word(const void *ptr) { return 0; };
static inline void eeprom_write_word(const void *ptr, uint16_t val) { };

static inline char *utoa(unsigned int num, char *str, int radix)
{
    char temp[17];  //an int can only be 16 bits long
                    //at radix 2 (binary) the string
                    //is at most 16 + 1 null long.
    int temp_loc = 0;
    int str_loc = 0;

    //construct a backward string of the number.
    do {
        int digit = (unsigned int)num % radix;
        if (digit < 10)
            temp[temp_loc++] = digit + '0';
        else
            temp[temp_loc++] = digit - 10 + 'A';
        num = ((unsigned int)num) / radix;
    } while ((unsigned int)num > 0);

    temp_loc--;


    //now reverse the string.
    while ( temp_loc >=0 ) {// while there are still chars
        str[str_loc++] = temp[temp_loc--];
    }
    str[str_loc] = 0; // add null termination.

    return str;
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <cstdlib>
static inline long random(long v)
{
    return rand() % v;
}

static inline long random(long min, long max)
{
    return rand() % (max - min + 1) + min;
}
#endif

#endif

