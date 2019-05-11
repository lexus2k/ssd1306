/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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

#include "intf/ssd1306_interface.h"
#include "intf/vga/vga.h"
#include "lcd/lcd_common.h"
#include "lcd/vga_commands.h"

#if defined(CONFIG_VGA_AVAILABLE) && defined(CONFIG_VGA_ENABLE) && defined(ESP32)

#include "CompositeOutput.h"

//#define VGA_CONTROLLER_DEBUG

static uint8_t *__vga_buffer = nullptr;
extern uint16_t ssd1306_color;

// Set to ssd1306 compatible mode by default
static uint8_t s_mode = 0x01;
static uint8_t s_vga_command = 0xFF;
static uint8_t s_vga_arg = 0;
static uint8_t s_column = 0;
static uint8_t s_column_end = 0;
static uint8_t s_cursor_x = 0;
static uint8_t s_cursor_y = 0;
static uint8_t s_width = 0;
static uint8_t s_height = 0;
static uint8_t s_bpp = 0;

static CompositeOutput output(CompositeOutput::PAL);

static void compositeCore(void *data)
{
    while (true)
    {
        //just send the graphics frontbuffer whithout any interruption 
        output.sendFrameHalfResolution(__vga_buffer);
    }
}

static void vga_controller_init(void)
{
    s_vga_command = 0xFF;
}

#ifdef VGA_CONTROLLER_DEBUG
#include <stdio.h>
void uart_send_byte(uint8_t data)
{
    printf("%c", data);
}
#endif

static void vga_controller_stop(void)
{
    s_vga_command = 0xFF;
    #ifdef VGA_CONTROLLER_DEBUG
        ssd1306_debug_print_vga_buffer( uart_send_byte );
    #endif
}

static void vga_controller_close(void)
{
}

/*
 * Function sends 8 vertical pixels to buffer
 */
static inline void vga_controller_put_pixels(uint8_t x, uint8_t y, uint8_t pixels)
{
    uint16_t addr = (x >> 3)   + (uint16_t)(y * 16);
    uint8_t offset = x & 0x07;
    uint8_t mask = 1 << offset;
    if (addr >= 16*64)
    {
        return;
    }
    for (uint8_t i=8; i>0; i--)
    {
        if (pixels & 0x01) __vga_buffer[addr] |= mask;
                      else __vga_buffer[addr] &= ~mask;
        addr += 16;
        pixels >>= 1;
    }
}

static void vga_controller_send_byte(uint8_t data)
{
    if (s_vga_command == 0xFF)
    {
        s_vga_command = data;
        return;
    }
    if (s_vga_command == 0x40)
    {
        vga_controller_put_pixels(s_cursor_x, s_cursor_y, data);
        s_cursor_x++;
        if (s_cursor_x > s_column_end)
        {
            s_cursor_x = s_column;
            s_cursor_y += 8;
        }
        return;
    }
    // command mode
    if (!s_vga_command)
    {
        s_vga_command = data;
        s_vga_arg = 0;
    }
    if (s_vga_command == VGA_SET_BLOCK)
    {
        // set block
        if (s_vga_arg == 1)
        {
            s_column = data >= ssd1306_lcd.width ? ssd1306_lcd.width - 1 : data ;
            s_cursor_x = s_column;
        }
        if (s_vga_arg == 2) { s_column_end = data >= ssd1306_lcd.width ? ssd1306_lcd.width - 1 : data; }
        if (s_vga_arg == 3) { s_cursor_y = (data << 3); }
        if (s_vga_arg == 4) { s_vga_command = 0; }
    }
    else if (s_vga_command == VGA_SET_MODE)
    {
        if (s_vga_arg == 1) { s_mode = data; s_vga_command = 0; }
        if (s_vga_arg == 2) { s_vga_command = 0; }
    }
    else if (s_vga_command == VGA_SET_RESOLUTION )
    {
        if (s_vga_arg == 1) { s_width = data; }
        if (s_vga_arg == 2) { s_height = data; }
        if (s_vga_arg == 3) { s_bpp = data; s_vga_command = 0; }
    }
    else if (s_vga_command == VGA_DISPLAY_ON )
    {
        __vga_buffer = (uint8_t *) malloc(s_width * s_height * s_bpp / 8);
        output.init(s_width, s_height, s_bpp);
        xTaskCreatePinnedToCore(compositeCore, "c", 1024, NULL, 1, NULL, 0);
        s_vga_command = 0;
    }
    s_vga_arg++;
}

static void vga_controller_send_bytes(const uint8_t *buffer, uint16_t len)
{
    while (len--)
    {
        ssd1306_intf.send(*buffer);
        buffer++;
    }
}

extern "C" void ssd1306_CompositeVideoInit_esp32(void);
void ssd1306_CompositeVideoInit_esp32(void)
{
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = vga_controller_init;
    ssd1306_intf.stop = vga_controller_stop;
    ssd1306_intf.send = vga_controller_send_byte;
    ssd1306_intf.send_buffer = vga_controller_send_bytes;
    ssd1306_intf.close = vga_controller_close;
}

void ssd1306_debug_print_vga_buffer_128x64(void (*func)(uint8_t))
{
    for(int y = 0; y < ssd1306_lcd.height; y++)
    {
        for(int x = 0; x < ssd1306_lcd.width; x++)
        {
            uint8_t color = __vga_buffer[(x >> 3) + y * 16] & (1<< (x&0x07));
            if (color)
            {
                func('#');
            }
            else
            {
                func(' ');
            }
        }
        func('\n');
    }
    func('\n');
}

void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t))
{
    ssd1306_debug_print_vga_buffer_128x64(func);
}

#endif
