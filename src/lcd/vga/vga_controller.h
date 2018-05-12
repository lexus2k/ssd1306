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

#include "vga_controller_base.h"
#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

// Including this header defines ISR handlers in your application automatically
// If you want to use your ISR's, please include "vga_controller_base.h"
#if defined(SSD1306_BUILTIN_VGA_SUPPORT) && !defined(VGA_CONTROLLER_DEBUG)

// Total number of lines used in specific scan mode
static uint16_t s_total_mode_lines = 40*8;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
static const uint8_t V_BACKPORCH_LINES = 40;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
volatile int s_current_scan_line;
volatile uint8_t s_lines_to_skip;
volatile uint8_t *s_current_scan_line_data = &s_vga_buffer[0];

// ISR: Vsync pulse
ISR(TIMER1_OVF_vect)
{
    s_current_scan_line = 0;
    s_current_scan_line_data = &s_vga_buffer[0];
    s_lines_to_skip = V_BACKPORCH_LINES;
} // end of TIMER1_OVF_vect

static inline void do_scan_line()
{
    // Implement all code directly in ISR: No any calls to subroutines!!!!
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
    #ifndef SSD1306_VGA_SLEEP_MODE
    // This is dejitter code, it purpose to start pixels output at the same offset after h-sync
    asm volatile(
      "     lds r16, %[timer0]    \n\t" //
      "     subi r16, -3           \n\t" // some offset, calculated experimentally
      "     andi r16, 7           \n\t" // use module 8 value from Timer 0 counter
      "     ldi r31, pm_hi8(LW)   \n\t" // load label address
      "     ldi r30, pm_lo8(LW)   \n\t" //
      "     add r30, r16          \n\t" // no need to multiply by 2 since AVR addresses are half-values
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
    : [timer0] "i" (&TCNT0)
    : "r30", "r31", "r16", "r17");
    #endif
    // output all pixels
    asm volatile(
         "ldi r20, 16\n\t"
         ".rept 32\n\t"
         "ld r16, Z+\n\t"
         "nop\n\t"               // to make pixel wider in 64x40 mode
         "nop\n\t"               // to make pixel wider in 64x40 mode
         "out %[port], r16\n\t"
         "mul r16, r20\n\t"
         "nop\n\t"               // to make pixel wider in 64x40 mode
         "nop\n\t"               // to make pixel wider in 64x40 mode
         "out %[port], r1\n\t"
         ".endr\n\t"
         "nop\n\t"               // to make pixel wider in 64x40 mode
         "nop\n\t"               // to make pixel wider in 64x40 mode
         "nop\n\t"
         "ldi r16,0\n\t"
         "out %[port], r16 \n\t"
         "nop\n\t"
    :
    : [port] "I" (_SFR_IO_ADDR(PORTC)),
       "z" "I" ((uint8_t *)s_current_scan_line_data )
    : "r16", "r17", "r20", "r21", "memory"
    );

    s_current_scan_line++;
    if ((s_current_scan_line & 0x7) == 0)
    {
        s_current_scan_line_data+=32;
    }
}

ISR(TIMER2_COMPB_vect) // for end of h-sync pulse
//ISR(TIMER2_OVF_vect) // for start of h-sync pulse
{
    do_scan_line();
}

#endif

#ifdef __cplusplus
}
#endif

#endif

