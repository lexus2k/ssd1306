/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SSD1306_I2C_CONF_H_
#define _SSD1306_I2C_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

// Uncomment this block only, if you need to use "embedded" I2C on Atmega controllers
// #define SSD1306_EMBEDDED_I2C

#ifndef SSD1306_SA
    /**
     * SSD1306_SA defines I2C address of LCD display. Please, check your device.
     * If you LCD device has different address, change it here.
     * Write command will be SSD1306_SA<<1 and read will be SSD1306_SA<<1 | 1
     */
    #define SSD1306_SA    0x3C  // Slave address
#endif

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#ifndef SSD1306_EMBEDDED_I2C
    /**
     * Use embedded i2c on attiny controllers. Wire library is not applicable
     */
    #define SSD1306_EMBEDDED_I2C
#endif
#endif

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) 
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   PB3 // SCL, Pin 3 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   PB4 // SDA, Pin 4 on SSD1306 Board
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