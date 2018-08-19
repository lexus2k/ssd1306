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
 *   !!!!!!!!!!!!!!!!!!         RUNS ON ATMEGA328P ONLY   !!!!!!!!!!!!!!!!!!!!
 *   Nano/Atmega328 PINS:
 *     A0 - blue channel of D-Sub connector
 *     A1 - green channel of D-Sub connector
 *     A2 - red channel of D-Sub connector
 *     D9 - H-Sync of D-Sub connector
 *     D3 - V-Sync of D-Sub connector
 *
 *   Sketch allows to use all other PINs except A3-A6 pins.
 *
 *   CONTROLLER will wait for commands over UART RX line.
 *
 *   Nano/Atmega328 PINS:
 *     RX - connect RX pin to TX pin of other board and run vga_client_demo on it.
 *
 */

#include "ssd1306.h"
//#define VGA_CONTROLLER_DEBUG
//#define SSD1306_VGA_SLEEP_MODE
#define CONFIG_VGA_96X40_ENABLE
#include "intf/vga/atmega328p/vga_isr.h"
#include "lcd/vga_monitor.h"
#include "intf/ssd1306_interface.h"

// There are some issues if UART interrupt enabled with VGA output
//#define UART_INTERRUPT_ENABLE
#include "ssd1306_uart.h"

/////////////////////////// UART /////////////////////////////////

enum
{
    UART_CMD_NONE      = 0x00, // Module is waiting for any input on UART
    UART_CMD_PROXY     = 0x01, // Module proxies all bytes to vga library interface
    UART_CMD_LISTEN    = 0x02, // Module waits for command over UART
    UART_CMD_ESCAPE    = 0x7D, // Escape symbol, next needs to be modified via ^0x20
    UART_CMD_FRAME_END = 0x7E, // Stop communication
};

/** Warning. May hold cpu while waiting for byte after 0x7D */
void vga_uart_on_receive(uint8_t data)
{
    static uint8_t s_uart_command = UART_CMD_NONE;
    static uint8_t s_uart_arg = 0;
    for(;;)
    {
        if (data == UART_CMD_FRAME_END)
        {
            if (s_uart_command != UART_CMD_NONE)
            {
                s_uart_command = UART_CMD_NONE;
                ssd1306_intf.stop();
                #ifdef VGA_CONTROLLER_DEBUG
                    ssd1306_debug_print_vga_buffer(uart_send_byte);
                #endif
            }
            else
            {
                ssd1306_intf.start();
            }
            return;
        }
        if (data == UART_CMD_ESCAPE)
        {
            while (!uart_byte_available());
            data = uart_read_byte() ^ 0x20;
        }
        break;
    }
    if (s_uart_command == UART_CMD_NONE)
    {
        if ((data == 0x40) || (data == 0x00))
        {
            // Proxy standard LCD controller commands vga interface
            ssd1306_intf.send(data);
            s_uart_command = UART_CMD_PROXY;
            return;
        }
        s_uart_command = data;
        s_uart_arg = 0;
    }
    if (s_uart_command == UART_CMD_PROXY)
    {
        ssd1306_intf.send(data);
        return;
    }
    // command mode
    if (s_uart_command == UART_CMD_LISTEN)
    {
        s_uart_command = data;
        s_uart_arg = 0;
    }
    if (s_uart_command == 'C')
    {
        ssd1306_clearScreen();
        ssd1306_setCursor(0,0);
        s_uart_command = UART_CMD_LISTEN;
    }
    if (s_uart_command == 'T')
    {
        if (s_uart_arg) ssd1306_write(data);
    }
    if (s_uart_command == 'M')
    {
        if (s_uart_arg)
        {
            if (data >= '0') data -= '0';
            ssd1306_setMode(static_cast<lcd_mode_t>(data));
            s_uart_command = UART_CMD_LISTEN;
        }
    }
    s_uart_arg++;
}

/////////////////////////////// MAIN /////////////////////////////////////

void setup()
{
    uart_init(57600);
//    uart_init(115200);
    ssd1306_vga_controller_init();
    vga_96x40_8colors_init();

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    ssd1306_printFixed(10,8,"SSD1306",STYLE_BOLD);
    ssd1306_printFixed(24,16,"by",STYLE_BOLD);
    ssd1306_printFixed(4,24,"Alexey D.",STYLE_BOLD);
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    ssd1306_drawRect(0,0,95,39);
    #ifdef VGA_CONTROLLER_DEBUG
        ssd1306_debug_print_vga_buffer(uart_send_byte);
    #endif
}

void loop()
{
    #ifdef SSD1306_VGA_SLEEP_MODE
    sleep_mode();
    #endif
    if (uart_byte_available())
    {
        uint8_t data = uart_read_byte();
        vga_uart_on_receive(data);
    }
}
