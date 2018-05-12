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

#ifdef SSD1306_UART_SUPPORTED

volatile uint8_t g_uart_put_ptr = 0;
volatile uint8_t g_uart_buf[UART_BUFFER_RX];
static uint8_t s_uart_get_ptr = 0;
static uint8_t s_uart_interrupt = 0;

void uart_init_internal(uint32_t baud, uint8_t interrupt)
{
    uint16_t ubrr0 = (((F_CPU) + 8UL * (baud)) / (16UL * (baud)) -1UL);
    s_uart_interrupt = interrupt;

    UBRR0H = ubrr0 >> 8;
    UBRR0L = ubrr0 & 0xFF;
    // no double speed
    UCSR0A &= ~(_BV(U2X0));

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
    if (s_uart_interrupt) return g_uart_put_ptr != s_uart_get_ptr;
    return uart_byte_available_direct();
}

static inline uint8_t uart_read_byte_direct(void)
{
    return UDR0;
}

uint8_t uart_read_byte(void)
{
    if (s_uart_interrupt)
    {
        uint8_t data = g_uart_buf[s_uart_get_ptr];
        s_uart_get_ptr = (s_uart_get_ptr + 1) & (UART_BUFFER_RX - 1);
        return data;
    }
    return uart_read_byte_direct();
}

#endif

