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


#include "ssd1306_uart_builtin.h"
#include "intf/ssd1306_interface.h"

#include "ssd1306_hal/io.h"

#if defined(CONFIG_AVR_UART_AVAILABLE) && defined(CONFIG_AVR_UART_ENABLE)

#include "ssd1306_uart.h"

static void ssd1306_uartStart(void)
{
}

static void ssd1306_uartStop(void)
{
    uart_send_byte(0x7E);
}

static void ssd1306_uartSendByte(uint8_t data)
{
    if ((data == 0x7E) || (data == 0x7D))
    {
         uart_send_byte(0x7D);
         data ^= 0x20;
    }
    uart_send_byte(data);
}

static void ssd1306_uartSendBytes(const uint8_t * buffer, uint16_t len)
{
    while (len--)
    {
        ssd1306_uartSendByte(*buffer);
        buffer++;
    }
}

void ssd1306_uartInit_Builtin(uint32_t baud)
{
    if (!baud) baud = 115200;
    uart_init(baud);
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = ssd1306_uartStart;
    ssd1306_intf.stop = ssd1306_uartStop;
    ssd1306_intf.send = ssd1306_uartSendByte;
    ssd1306_intf.send_buffer = ssd1306_uartSendBytes;
    ssd1306_intf.close = ssd1306_uartStart;
}

#endif
