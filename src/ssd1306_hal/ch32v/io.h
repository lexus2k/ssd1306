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
 * @file ssd1306_hal/ch32v/io.h This is ch32v platform file
 */

#ifndef _SSD1306_CH32V_IO_H_
#define _SSD1306_CH32V_IO_H_

#define SSD1306_CH32V_PLATFORM

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* If your platform already has these definitions, comment out lines below */
#define LOW    0
#define HIGH   1
#define INPUT  0
#define OUTPUT 1

/* Progmem attribute for data, located in Flash */
#define PROGMEM

#define CONFIG_PLATFORM_I2C_AVAILABLE
// #define CONFIG_PLATFORM_SPI_AVAILABLE // TODO

#ifdef __cplusplus
extern "C" {
#endif

static inline int digitalRead(int pin) // digitalRead()
{
  return LOW;
}

static inline void digitalWrite(int pin, int level) // digitalWrite()
{
}

static inline void pinMode(int pin, int mode) // pinMode()
{
}

static inline int analogRead(int pin) // analogRead()
{
  return 0;
}

static inline uint32_t millis(void) // millis()
{
  return 0;
}

static inline uint32_t micros(void) // micros()
{
  return 0;
};

void delay(uint32_t ms);

static inline void delayMicroseconds(uint32_t us) // delayMicroseconds()
{
}

static inline uint8_t pgm_read_byte(const void *ptr) // pgm_read_byte() - can be left as is
{
  return *((const uint8_t *)ptr);
}

#ifdef __cplusplus
}
#endif

#endif
