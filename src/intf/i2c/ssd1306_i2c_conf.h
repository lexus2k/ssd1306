/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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
 * @file ssd1306_i2c_conf.h SSD1306 library basic i2c definitions
 */

#ifndef _SSD1306_I2C_CONF_H_
#define _SSD1306_I2C_CONF_H_

#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SSD1306_SA
    /**
     * @ingroup LCD_HW_INTERFACE_API
     *
     * SSD1306_SA defines default i2c address of LCD display. Please, check your device.
     * If you LCD device has different address, you can set different one via
     * ssd1306_i2cInit_Wire() or ssd1306_i2cInit_Embedded() functions.
     * Write command will be SSD1306_SA<<1 and read will be SSD1306_SA<<1 | 1
     */
    #define SSD1306_SA    0x3C  // Slave address
#endif

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   3 ///< SCL, Pin 3 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 ///< SDA, Pin 4 on SSD1306 Board
    #endif
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   4 ///< SCL, Pin 4 - physical pin 9 of Attiny84
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   6 ///< SDA, Pin 6 - physical pin 7 of Attiny84
    #endif
#else
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   5 // SCL, Pin A5 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 // SDA, Pin A4 on SSD1306 Board
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif