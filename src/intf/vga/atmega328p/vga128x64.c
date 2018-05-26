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

#include "intf/vga/vga.h"
// Never include vga128x64_isr.h here!!!
#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"
#include "lcd/vga_commands.h"

#if defined(CONFIG_VGA_AVAILABLE) && defined(CONFIG_VGA_ENABLE) && defined(__AVR_ATmega328P__)

extern uint16_t ssd1306_color;

/* This buffer fits 128x64 pixels
   Each 8 pixels are packed to 3 bytes:

   BYTE1: B7 R2 G2 B2 B8 R1 G1 B1
   BYTE2: G7 R4 G4 B4 G8 R3 G3 B3
   BYTE3: R7 R6 G6 B6 R8 R5 G5 B5

   Yeah, a little bit complicated, but this allows to quickly unpack structure
*/

// Set to ssd1306 compatible mode by default
static uint8_t s_mode = 0x01;
static uint8_t s_vga_command = 0xFF;
static uint8_t s_vga_arg = 0;
static uint8_t s_column = 0;
static uint8_t s_column_end = 0;
static uint8_t s_cursor_x = 0;
static uint8_t s_cursor_y = 0;
volatile uint8_t s_vga_frames;

static void vga_controller_init(void)
{
    s_vga_command = 0xFF;
}

static void vga_controller_stop(void)
{
    s_vga_command = 0xFF;
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
    if (s_vga_command == VGA_SET_MODE)
    {
        if (s_vga_arg == 1) { s_mode = data; s_vga_command = 0; }
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

static inline void init_vga_crt_driver(uint8_t enable_jitter_fix)
{
    cli();
    if (enable_jitter_fix)
    {
        // Configure Timer 0 to calculate jitter fix
        TIMSK0=0;
        TCCR0A=0;
        TCCR0B=1;
        OCR0A=0;
        OCR0B=0;
        TCNT0=0;
    }
    else
    {
        // Sorry, we still need to disable timer0 interrupts to avoid wake up in sleep mode
        TIMSK0 &= ~(1<<TOIE0);
    }

    // Timer 1 - vertical sync pulses
    pinMode (V_SYNC_PIN, OUTPUT);
    TCCR1A=(1<<WGM10) | (1<<WGM11) | (1<<COM1B1);
    TCCR1B=(1<<WGM12) | (1<<WGM13) | (1<<CS12) | (1<<CS10); //1024 prescaler
    OCR1A = 259;  // 16666 / 64 us = 260 (less one)
    OCR1B = 0;    // 64 / 64 us = 1 (less one)
    TIFR1 = (1<<TOV1);   // clear overflow flag
    TIMSK1 = (1<<TOIE1);  // interrupt on overflow on timer 1

    // Timer 2 - horizontal sync pulses
    pinMode (H_SYNC_PIN, OUTPUT);
    TCCR2A=(1<<WGM20) | (1<<WGM21) | (1<<COM2B1); //pin3=COM2B1
    TCCR2B=(1<<WGM22) | (1<<CS21); //8 prescaler
    OCR2A = 63;   // 32 / 0.5 us = 64 (less one)
    OCR2B = 7;    // 4 / 0.5 us = 8 (less one)
//    if (enable_jitter_fix)
    {
        TIFR2 = (1<<OCF2B);   // on end of h-sync pulse
        TIMSK2 = (1<<OCIE2B); // on end of h-sync pulse
    }
//    else
//    {
//        TIFR2 = (1<<TOV2);    // int on start of h-sync pulse
//        TIMSK2 = (1<<TOIE2);  // int on start of h-sync pulse
//    }

    // Set up USART in SPI mode (MSPIM)

    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(16, OUTPUT);
    PORTC = 0;

    sei();
}

void ssd1306_vga_controller_128x64_init_no_output(void)
{
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = vga_controller_init;
    ssd1306_intf.stop = vga_controller_stop;
    ssd1306_intf.send = vga_controller_send_byte;
    ssd1306_intf.send_buffer = vga_controller_send_bytes;
    ssd1306_intf.close = vga_controller_close;
}

void ssd1306_vga_controller_128x64_init_enable_output(void)
{
    ssd1306_vga_controller_128x64_init_no_output();
    init_vga_crt_driver(1);
}

void ssd1306_vga_controller_128x64_init_enable_output_no_jitter_fix(void)
{
    ssd1306_vga_controller_128x64_init_no_output();
    init_vga_crt_driver(0);
//    set_sleep_mode (SLEEP_MODE_IDLE);
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
                func('#');
            }
            else
            {
                func(' ');
                func(' ');
            }
        }
        func('\n');
    }
    func('\n');
}

#endif
