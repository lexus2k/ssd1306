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

#include "ssd1306_interface.h"
#include "spi/ssd1306_spi.h"
#include <stddef.h>

static void ssd1306_send_buffer_generic(const uint8_t* buffer, uint16_t size);

ssd1306_interface_t ssd1306_intf =
{
    .send_buffer = ssd1306_send_buffer_generic
};

void ssd1306_commandStart(void)
{
    ssd1306_intf.start();
    if (ssd1306_intf.spi)
        ssd1306_spiDataMode(0);
    else
        ssd1306_intf.send(0x00);
}

void ssd1306_dataStart(void)
{
    ssd1306_intf.start();
    if (ssd1306_intf.spi)
        ssd1306_spiDataMode(1);
    else
        ssd1306_intf.send(0x40);
}

void ssd1306_sendCommand(uint8_t command)
{
    ssd1306_commandStart();
    ssd1306_intf.send(command);
    ssd1306_intf.stop();
}

void ssd1306_send_buffer_generic(const uint8_t* buffer, uint16_t size)
{
    while (size--)
    {
        ssd1306_intf.send(*buffer);
        buffer++;
    }
}
