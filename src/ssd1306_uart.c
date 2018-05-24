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
 * @file nano_engine.h Small graphics engine, based on SSD1331 functions
 */

#include "ssd1306_uart.h"

#if defined(CONFIG_AVR_UART_AVAILABLE) && defined(CONFIG_AVR_UART_ENABLE)

volatile uint8_t g_uart_put_ptr = 0;
volatile uint8_t g_uart_buf[UART_BUFFER_RX];
static uint8_t s_uart_get_ptr = 0;
static uint8_t s_uart_interrupt = 0;

#undef BAUD
#define BAUD 115200
// We need to define BAUD_TOL too high, because otherwise,
// the compiler will give a warning "Baud rate achieved is higher than allowed"
// I would recommend not to use 115200 on atmega328p
#define BAUD_TOL 4
#include <util/setbaud.h>
static const uint8_t ubrr0h_115200 = UBRRH_VALUE;
static const uint8_t ubrr0l_115200 = UBRRL_VALUE;
#ifdef USE_2X
static const uint8_t u2x0_115200 = 1;
#else
static const uint8_t u2x0_115200 = 0;
#endif

#undef BAUD
#define BAUD 57600
#include <util/setbaud.h>
static const uint8_t ubrr0h_57600 = UBRRH_VALUE;
static const uint8_t ubrr0l_57600 = UBRRL_VALUE;
#ifdef USE_2X
static const uint8_t u2x0_57600 = 1;
#else
static const uint8_t u2x0_57600 = 0;
#endif

#undef BAUD
#define BAUD 38400
#include <util/setbaud.h>
static const uint8_t ubrr0h_38400 = UBRRH_VALUE;
static const uint8_t ubrr0l_38400 = UBRRL_VALUE;
#ifdef USE_2X
static const uint8_t u2x0_38400 = 1;
#else
static const uint8_t u2x0_38400 = 0;
#endif

#undef BAUD
#define BAUD 19200
#include <util/setbaud.h>
static const uint8_t ubrr0h_19200 = UBRRH_VALUE;
static const uint8_t ubrr0l_19200 = UBRRL_VALUE;
#ifdef USE_2X
static const uint8_t u2x0_19200 = 1;
#else
static const uint8_t u2x0_19200 = 0;
#endif

void uart_init_internal(uint32_t baud, uint8_t interrupt)
{
    s_uart_interrupt = interrupt;
    switch (baud)
    {
        case 19200:
            UBRR0H = ubrr0h_19200;
            UBRR0L = ubrr0l_19200;
            if (u2x0_19200) UCSR0A |= _BV(U2X0); else UCSR0A &= ~(_BV(U2X0));
            break;
        case 38400:
            UBRR0H = ubrr0h_38400;
            UBRR0L = ubrr0l_38400;
            if (u2x0_38400) UCSR0A |= _BV(U2X0); else UCSR0A &= ~(_BV(U2X0));
            break;
        case 57600:
            UBRR0H = ubrr0h_57600;
            UBRR0L = ubrr0l_57600;
            if (u2x0_57600) UCSR0A |= _BV(U2X0); else UCSR0A &= ~(_BV(U2X0));
            break;
        case 115200:
            UBRR0H = ubrr0h_115200;
            UBRR0L = ubrr0l_115200;
            if (u2x0_115200) UCSR0A |= _BV(U2X0); else UCSR0A &= ~(_BV(U2X0));
            break;
        default:
            break;
    }
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | (s_uart_interrupt ? _BV(RXCIE0) : 0);
}

void uart_send_byte(uint8_t c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

static inline uint8_t uart_byte_available_direct(void)
{
    return bit_is_set(UCSR0A, RXC0);
}

uint8_t uart_byte_available(void)
{
    if (!s_uart_interrupt)
    {
        while (uart_byte_available_direct()) __uart_read_byte();
    }
    return g_uart_put_ptr != s_uart_get_ptr;
}

uint8_t uart_read_byte(void)
{
    uint8_t data = g_uart_buf[s_uart_get_ptr];
    s_uart_get_ptr = (s_uart_get_ptr + 1) & (UART_BUFFER_RX - 1);
    return data;
}

#endif

