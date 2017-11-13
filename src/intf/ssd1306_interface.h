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
 * @file ssd1306_interface.h SSD1306 interface functions.
 */


#ifndef _SSD1306_INTERFACE_H_
#define _SSD1306_INTERFACE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

/**
 * Starts communication with SSD1306 display.
 */
extern void  (*ssd1306_startTransmission)();

/**
 * Ends communication with SSD1306 display.
 */
extern void  (*ssd1306_endTransmission)();

/**
 * Sends byte to SSD1306 device
 * @param data - byte to send
 */
extern void  (*ssd1306_sendByte)(uint8_t data);

/**
 * Sends command to SSD1306 device: includes initiating of
 * transaction, sending data and completing transaction.
 * @param command - command to send
 */
void ssd1306_sendCommand(uint8_t command);

/**
 * Starts transaction for sending commands.
 */
extern void (*ssd1306_commandStart)();

/**
 * Starts transaction for sending bitmap data.
 */
extern void (*ssd1306_dataStart)();

/**
 * Sends byte data to SSD1306 controller memory.
 * Performs 3 operations at once: ssd1306_dataStart(); ssd1306_sendByte( data ); ssd1306_endTransmission();
 * @param data - byte to send to the controller memory
 */
void         ssd1306_sendData(uint8_t data);

/**
 * Sets block in RAM of lcd display controller to write data to.
 * For ssd1306 it uses horizontal addressing mode, while for
 * sh1106 the function uses page addressing mode.
 * Width can be specified as 0, thus the library will set the right
 * region of RAM block to the right column of the display.
 * @param x - column (left region)
 * @param y - page (top page of the block)
 * @param w - width of the block in pixels to control
 */
extern void (*ssd1306_setRamBlock)(uint8_t x, uint8_t y, uint8_t w);

/**
 * Switches to the start of next RAM page for the block, specified by
 * ssd1306_setRamBlock().
 * For ssd1306 it does nothing, while for sh1106 the function moves cursor to
 * next page.
 */
extern void (*ssd1306_nextRamPage)();

/**
 * Sets position in RAM of lcd display controller to write data to.
 * For ssd1306 this function is not defined. So, calling it will cause
 * your controller to reset. For sh1106 the function does the same as
 * ssd1306_setRamBlock().
 * @param x - column (left region)
 * @param y - page (top page of the block)
 */
extern void (*ssd1306_setRamPos)(uint8_t x, uint8_t y);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_INTERFACE_H_
