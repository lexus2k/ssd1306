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
 * @file ssd1306_uart_builtin.h embedded SSD1306 uart communication functions
 */

#ifndef _SSD1306_UART_BUILTIN_H_
#define _SSD1306_UART_BUILTIN_H_

#include "ssd1306_hal/io.h"

#if defined(CONFIG_AVR_UART_AVAILABLE) && defined(CONFIG_AVR_UART_ENABLE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Initializes built-in implementation of uart functions, based on USART hw module.
 * If you do not know uart parameters, try ssd1306_uartInit_Builtin(0).
 * @param baud uart baud rate
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void ssd1306_uartInit_Builtin(uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_UART_BUILTIN_H_ */


