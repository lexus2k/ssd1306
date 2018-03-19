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
 * @defgroup LCD_HW_INTERFACE_API LCD physical interface functions
 * @{
 *
 * @brief i2c/spi initialization functions for different platforms
 *
 * @details This group of API functions serves to prepare the library to work via specific hardware
 *          interface. There are a bunch of functions for different platforms. In general display
 *          initialization goes in two steps: hardware interface initialization, and then display 
 *          driver initialization. But there are functions, which combine 2 steps in single call:
 *          ssd1306_128x64_i2c_initEx(), ssd1351_128x128_spi_init(), etc.
 */

/**
 * Indicates if display driver supports quick switching between data and command modes:
 * 0 means "not supported quick switching", 1 means "quick switching is supported".
 * Quick switching allows to switch driver between data and command modes without
 * reestablishing communication session. For all i2c interfaces quick switching is not supported,
 * because data or command mode is defined by first sent byte in i2c session.
 * All spi interfaces support quick switching.
 */
extern uint8_t ssd1306_dcQuickSwitch;

/**
 * Indicates if display driver supports quick switching between data and command modes:
 * 0 means "not supported quick switching", 1 means "quick switching is supported".
 * Quick switching allows to switch driver between data and command modes without
 * reestablishing communication session. For all i2c interfaces quick switching is not supported,
 * because data or command mode is defined by first sent byte in i2c session.
 * All spi interfaces support quick switching.
 */
extern uint8_t ssd1306_dcQuickSwitch;

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
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_INTERFACE_H_
