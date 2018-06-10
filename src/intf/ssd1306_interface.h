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

#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_HW_INTERFACE_API I2C/SPI: physical interface functions
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

/** Describes low level hardware API */
typedef struct
{
    /**
     * Indicates if spi or i2c interface is used.
     */
    uint8_t spi;
    /**
     * Starts communication with SSD1306 display.
     */
    void (*start)(void);
    /**
     * Ends communication with SSD1306 display.
     */
    void (*stop)(void);
    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void (*send)(uint8_t data);
    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void (*send_buffer)(const uint8_t *buffer, uint16_t size);
    /**
     * @brief deinitializes internal resources, allocated for interface.
     *
     * Deinitializes internal resources, allocated for interface.
     * There is no need to use this function for microcontrollers. In general
     * the function has meaning in Linux-like systems.
     */
    void (*close)(void);
} ssd1306_interface_t;

/**
 * Holds pointers to functions of currently initialized interface.
 */
extern ssd1306_interface_t ssd1306_intf;

/**
 * Deprecated
 */
#define ssd1306_dcQuickSwitch ssd1306_intf.spi

/**
 * Deprecated
 */
#define ssd1306_startTransmission     ssd1306_intf.start

/**
 * Deprecated
 */
#define ssd1306_endTransmission       ssd1306_intf.stop

/**
 * Deprecated
 */
#define ssd1306_sendByte              ssd1306_intf.send

/**
 * Deprecated
 */
#define ssd1306_sendBytes             ssd1306_intf.send_buffer

/**
 * Deprecated
 */
#define ssd1306_closeInterface        ssd1306_intf.close

/**
 * Sends command to SSD1306 device: includes initiating of
 * transaction, sending data and completing transaction.
 * @param command - command to send
 */
void ssd1306_sendCommand(uint8_t command);

/**
 * Starts transaction for sending commands.
 */
void ssd1306_commandStart(void);

/**
 * Starts transaction for sending bitmap data.
 */
void ssd1306_dataStart(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_INTERFACE_H_
