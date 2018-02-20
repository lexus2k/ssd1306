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

#if (defined(__linux__) || defined(__MINGW32__)) && !defined(ARDUINO)

#if defined(__KERNEL__)
#include <linux/types.h>
#elif defined(__MINGW32__)
#include <windows.h>
#include <stdint.h>
#include <unistd.h>
#include "sdl_core.h"
#else
#include <stdint.h>
#include <unistd.h>
#endif
#include <time.h>

#define LOW  0
#define HIGH 1
#define INPUT 1
#define OUTPUT 0
#define PROGMEM

#define SSD1306_LINUX_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

static inline void digitalWrite(int pin, int level) {};
static inline int  digitalRead(int pin) { return LOW; };
static inline void pinMode(int pin, int mode) {};
#if defined(__KERNEL__)
static inline void delay(unsigned long ms) {  };
#elif defined(__MINGW32__)
static inline void delay(unsigned long ms) { Sleep(ms);  };
static inline void delayMicroseconds(unsigned long us) { Sleep((us+500)/1000); };
#else
static inline void delay(unsigned long ms) { usleep(ms*1000);  };
static inline void delayMicroseconds(unsigned long us) { usleep(us); };
#endif

#if defined(__KERNEL__)
static inline int  analogRead(int pin) { return 0; };
#elif defined(__MINGW32__)
static inline int  analogRead(int pin) { return sdl_read_analog(pin); };
#else
static inline int  analogRead(int pin) { return 0; };
#endif

#if defined(__KERNEL__)
static inline uint32_t millis(void) { return 0; };
static inline uint32_t micros(void) { return 0; };

#elif defined(__MINGW32__)
static inline uint32_t millis(void)
{
    return GetTickCount();
};

static inline uint32_t micros(void)
{
    return GetTickCount()*1000;
};

#else
static inline uint32_t millis(void)
{
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
};

static inline uint32_t micros(void)
{
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
};
#endif


static inline void randomSeed(int seed) { };
static inline void attachInterrupt(int pin, void (*interrupt)(void), int level) { };
static inline uint8_t pgm_read_byte(const void *ptr) { return *((const uint8_t *)ptr); };
static inline uint16_t eeprom_read_word(const void *ptr) { return 0; };
static inline void eeprom_write_word(const void *ptr, uint16_t val) { };

#if !defined(__MINGW32__)
/* For some reason defines do not work accross the libraries *
 * Didn't yet figure out, what is the reason fo this issue */
//#define min(a,b) (((a)<(b))?(a):(b))
//#define max(a,b) (((a)>(b))?(a):(b))
static inline int min(int a, int b) { return a<b?a:b; };
static inline int max(int a, int b) { return a>b?a:b; };
#endif

static inline char *utoa(unsigned int num, char *str, int radix) {
    char temp[17];  //an int can only be 16 bits long
                    //at radix 2 (binary) the string
                    //is at most 16 + 1 null long.
    int temp_loc = 0;
    int digit;
    int str_loc = 0;

    //construct a backward string of the number.
    do {
        digit = (unsigned int)num % radix;
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
static inline int random(int max) { return 0; };
static inline int random(int min, int max) { return 0; };
#endif

#endif

#endif

