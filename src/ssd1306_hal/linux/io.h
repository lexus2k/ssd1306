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

/*
 * @file ssd1306_hal/linux/io.h SSD1306 LINUX IO communication functions
 */

#ifndef _SSD1306_LINUX_IO_H_
#define _SSD1306_LINUX_IO_H_

#define CONFIG_PLATFORM_I2C_AVAILABLE
#define CONFIG_PLATFORM_SPI_AVAILABLE

#include "ssd1306_hal/UserSettings.h"

#if defined(SDL_EMULATION)  // SDL Emulation mode includes
#include "sdl_core.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
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

#include "ssd1306_hal/ssd1306_interface.h"

#if defined(SDL_EMULATION)  // SDL Emulation mode includes

class SdlI2c: public IWireInterface
{
public:
    SdlI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00);
    virtual ~SdlI2c();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;
private:
    int8_t m_scl;
    int8_t m_sda;
    uint8_t m_sa;
};

class PlatformI2c: public SdlI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
        SdlI2c(scl, sda, sa) {}
};


class SdlSpi: public IWireInterface
{
public:
    SdlSpi(int8_t dcPin = -1);
    virtual ~SdlSpi();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;
private:
    int8_t m_cs;
    int8_t m_dc;
    uint32_t m_frequency;
};

class PlatformSpi: public SdlSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        SdlSpi(dcPin) {}
};

#else

class LinuxI2c: public IWireInterface
{
public:
    LinuxI2c(int8_t busId = -1, uint8_t sa = 0x00);
    virtual ~LinuxI2c();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;
private:
    int8_t m_busId;
    uint8_t m_sa;
    int m_fd;
    uint16_t m_dataSize;
    uint8_t m_buffer[1024];
};

class PlatformI2c: public LinuxI2c
{
public:
    PlatformI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00):
         LinuxI2c( scl, sa ) {}
};


class LinuxSpi: public IWireInterface
{
public:
    LinuxSpi(int busId, int8_t devId, int8_t dcPin, uint32_t frequency);

    virtual ~LinuxSpi();

    /**
     * Starts communication with SSD1306 display.
     */
    void start() override;

    /**
     * Ends communication with SSD1306 display.
     */
    void stop() override;

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data) override;

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size) override;

private:
    int m_busId;
    int8_t m_devId;
    int8_t m_dc;
    uint32_t m_frequency;
    uint16_t m_spi_cached_count;
    uint8_t m_spi_cache[1024];
    int m_spi_fd;

    void sendCache();
    static void OnDcChange(void *arg);
};

class PlatformSpi: public LinuxSpi
{
public:
    PlatformSpi(int8_t csPin = -1, int8_t dcPin = -1, uint32_t freq = 0):
        LinuxSpi( 0, csPin, dcPin, freq ) {}
};

#endif

extern "C" {
#endif

#if defined(__KERNEL__)      // ============== KERNEL
static inline int  digitalRead(int pin) { return LOW; };
static inline void delay(unsigned long ms) {  };
static inline void delayMicroseconds(unsigned long us) {  };
static inline int  analogRead(int pin) { return 0; };
static inline void digitalWrite(int pin, int level) { };
static inline uint32_t millis(void) { return 0; };
static inline uint32_t micros(void) { return 0; };
static inline void pinMode(int pin, int mode) {};

#else                         // ============== LINUX
static inline void delay(unsigned long ms) { usleep(ms*1000);  };
static inline void delayMicroseconds(unsigned long us) { usleep(us); };
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

/* For some reason defines do not work accross the libraries *
 * Didn't yet figure out, what is the reason fo this issue */
//define min(a,b) (((a)<(b))?(a):(b))
//define max(a,b) (((a)>(b))?(a):(b))
static inline int min(int a, int b) { return a<b?a:b; };
static inline int max(int a, int b) { return a>b?a:b; };
#if !defined(SDL_EMULATION)
void pinMode(int pin, int mode);
static inline int  digitalRead(int pin) { return LOW; };
#endif

#endif

#if defined(SDL_EMULATION)
static inline int  digitalRead(int pin) { return sdl_read_digital(pin); };
static inline int  analogRead(int pin) { return sdl_read_analog(pin); };
static inline void digitalWrite(int pin, int level) {  sdl_write_digital(pin, level); };
static inline void pinMode(int pin, int mode) { };
#elif !defined(__KERNEL__)
static inline int  analogRead(int pin) { return 0; };
void digitalWrite(int pin, int level);
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

