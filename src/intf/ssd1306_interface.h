/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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
 * @file ssd1306_interface.h SSD1306 interface functions.
 */


#ifndef _SSD1306_INTERFACE_H_
#define _SSD1306_INTERFACE_H_

#include "hal/io.h"

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
extern void  (*ssd1306_startTransmission)(void);

/**
 * Ends communication with SSD1306 display.
 */
extern void  (*ssd1306_endTransmission)(void);

/**
 * Sends byte to SSD1306 device
 * @param data - byte to send
 */
extern void  (*ssd1306_sendByte)(uint8_t data);

/**
 * @brief deinitializes internal resources, allocated for interface.
 *
 * Deinitializes internal resources, allocated for interface.
 * There is no need to use this function for microcontrollers. In general
 * the function has meaning in Linux-like systems.
 */
extern void  (*ssd1306_closeInterface)(void);

/**
 * Sends 8 monochrome vectical pixels to OLED driver.
 * @param data - byte, representing 8 pixels.
 */
extern void  (*ssd1306_sendPixels)(uint8_t data);

/**
 * Sends command to SSD1306 device: includes initiating of
 * transaction, sending data and completing transaction.
 * @param command - command to send
 */
void ssd1306_sendCommand(uint8_t command);

/**
 * Starts transaction for sending commands.
 */
extern void (*ssd1306_commandStart)(void);

/**
 * Starts transaction for sending bitmap data.
 */
extern void (*ssd1306_dataStart)(void);

/**
 * Sends byte data to SSD1306 controller memory.
 * Performs 3 operations at once: ssd1306_dataStart(); ssd1306_sendPixels( data ); ssd1306_endTransmission();
 * @param data - byte to send to the controller memory
 * @note At present this function is used only in Arkanoid demo.
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
extern void (*ssd1306_nextRamPage)(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_INTERFACE_H_
