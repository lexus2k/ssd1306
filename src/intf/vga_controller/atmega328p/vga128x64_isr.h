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
 * @file vga_controller.h VGA interface to communicate with VGA-monitor from the sketch directly
 *
 * @details Please, include this file only once in your sketch, if you want to control
 * vga monitor directly from your sketch
 * This header supports different modes: VGA_CONTROLLER_DEBUG, SSD1306_VGA_SLEEP_MODE.
 * If you want to use vga_controller module in debug mode without producing VGA output,
 * define VGA_CONTROLLER_DEBUG before including this header. If you want to use library with
 * AVR sleep mode, then jitter fix is not required, you will able to use TIMER0, so define
 * SSD1306_VGA_SLEEP_MODE before including this header.
 */

#ifndef _SSD1306_VGA_CONTROLLER_H_
#define _SSD1306_VGA_CONTROLLER_H_

#include "vga.h"
#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

// Including this header defines ISR handlers in your application automatically
#if defined(SSD1306_BUILTIN_VGA_SUPPORT) && defined(CONFIG_VGA_ENABLE)

#if !defined(VGA_CONTROLLER_DEBUG)

#ifndef DEJITTER_OFFSET
#define DEJITTER_OFFSET -4
#endif

extern volatile uint8_t s_vga_buffer_128x64_mono[];

// Total number of lines used in specific scan mode
static uint16_t s_total_mode_lines = 64*7;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
static const uint8_t V_BACKPORCH_LINES = 40;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
volatile int s_current_scan_line;
volatile uint8_t s_lines_to_skip;
volatile uint8_t s_scan_line_index;
volatile uint8_t *s_current_scan_line_data = &s_vga_buffer_128x64_mono[0];
extern volatile uint8_t s_vga_frames;
extern unsigned long timer0_millis;
// ISR: Vsync pulse
ISR(TIMER1_OVF_vect)
{
    s_current_scan_line = 0;
    s_scan_line_index = 0;
    s_current_scan_line_data = &s_vga_buffer_128x64_mono[0];
    s_lines_to_skip = V_BACKPORCH_LINES;
    s_vga_frames++;
    timer0_millis += 16;
} // end of TIMER1_OVF_vect

static inline void /*__attribute__ ((noinline))*/ do_scan_line()
{
    #ifndef SSD1306_VGA_SLEEP_MODE
    // This is dejitter code, it purpose to start pixels output at the same offset after h-sync
    asm volatile(
      "     lds r24, %[timer0]    \n\t" //
      "     subi r24, %[toffset]  \n\t" // some offset, calculated experimentally
      "     andi r24, 7           \n\t" // use module 8 value from Timer 0 counter
      "     ldi r31, pm_hi8(LW)   \n\t" // load label address
      "     ldi r30, pm_lo8(LW)   \n\t" //
      "     add r30, r24          \n\t" // no need to multiply by 2 since AVR addresses are half-values
      "     adc r31, __zero_reg__ \n\t" //
      "     ijmp                  \n\t" //
      "LW:                        \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
    :
    : [timer0] "i" (&TCNT0),
      [toffset] "i" ((uint8_t)DEJITTER_OFFSET)
    : "r30", "r31", "r24", "r25");
    #endif
    // output all pixels

    asm volatile(
         ".rept 16\n\t"

         "ld r24, Z+\n\t"      
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "nop\n\t"
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "nop\n\t"
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "nop\n\t"
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "nop\n\t"
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "nop\n\t"
         "out %[port], r24\n\t"
         "lsr r24\n\t"
         "nop\n\t"
         "out %[port], r24\n\t"

         ".endr\n\t"
         "nop\n\t"
         "ldi r24,0\n\t"
         "out %[port], r24 \n\t"
         "nop\n\t"
    :
    : [port] "I" (_SFR_IO_ADDR(PORTC)),
       "z" "I" ((uint8_t *)s_current_scan_line_data )
    : "r24", "memory"
    );

    s_current_scan_line++;
    s_scan_line_index++;
    if ( s_scan_line_index > 6 )
    {
        s_scan_line_index=0;
        s_current_scan_line_data += 16;
    }
}

//#ifdef SSD1306_VGA_SLEEP_MODE
//ISR(TIMER2_OVF_vect) // for start of h-sync pulse
//#else
ISR(TIMER2_COMPB_vect) // for end of h-sync pulse
//#endif
{
    // ISR should work as fast as possible
    if (s_lines_to_skip)
    {
        s_lines_to_skip--;
        return;
    }
    else if (s_current_scan_line >= s_total_mode_lines)
    {
        return;
    }
    do_scan_line();
}

#endif  // VGA_CONTROLLER_DEBUG

void ssd1306_vga_controller_128x64_init_no_output(void);
void ssd1306_vga_controller_128x64_init_enable_output(void);
void ssd1306_vga_controller_128x64_init_enable_output_no_jitter_fix(void);

static inline void ssd1306_vga_controller_128x64_init(void)
{
    // if there is no builtin support then only debug mode is available
#if defined(VGA_CONTROLLER_DEBUG)
    ssd1306_vga_controller_128x64_init_no_output();
#elif defined(SSD1306_VGA_SLEEP_MODE)
    ssd1306_vga_controller_128x64_init_enable_output_no_jitter_fix();
#else
    ssd1306_vga_controller_128x64_init_enable_output();
#endif
}

void ssd1306_vga_controller_init(void)
{
    ssd1306_vga_controller_128x64_init();
}

void ssd1306_debug_print_vga_buffer_128x64(void (*func)(uint8_t));

void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t))
{
    ssd1306_debug_print_vga_buffer_128x64(func);
}

#endif  // SSD1306_BUILTIN_VGA_SUPPORT

#ifdef __cplusplus
}
#endif

#endif

