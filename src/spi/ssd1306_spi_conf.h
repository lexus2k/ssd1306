/*
    Copyright (C) 2017 Alexey Dynda

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

/**
 * @file ssd1306_spi_conf.h SSD1306 library basic spi definitions
 */

#ifndef _SSD1306_SPI_CONF_H_
#define _SSD1306_SPI_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    /**
     * Use embedded spi on attiny controllers. SPI library is not applicable
     * The macro is deprecated. Use SSD1306_SPI_SUPPORTED instead.
     */
    #define SSD1306_EMBEDDED_SPI
#else
    /** The macro is defined when SPI library is available */
    #define SSD1306_SPI_SUPPORTED
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif