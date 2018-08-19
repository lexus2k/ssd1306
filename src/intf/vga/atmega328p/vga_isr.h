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
 * @file vga_isr.h VGA ISR code to communicate with VGA-monitor from the sketch directly
 *
 * @details Please, include this file only once in your sketch, if you want to control
 * vga monitor directly from your sketch.
 * This header supports different modes: VGA_CONTROLLER_DEBUG, SSD1306_VGA_SLEEP_MODE.
 * If you want to use vga_controller module in debug mode without producing VGA output,
 * define VGA_CONTROLLER_DEBUG before including this header. If you want to use library with
 * AVR sleep mode, then jitter fix is not required, you will able to use TIMER0, so define
 * SSD1306_VGA_SLEEP_MODE before including this header.
 */

#ifndef _SSD1306_VGA_ATMEGA328P_ISR_H_
#define _SSD1306_VGA_ATMEGA328P_ISR_H_

#include "intf/vga/vga.h"
#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

// Including this header defines ISR handlers in your application automatically
#if defined(CONFIG_VGA_AVAILABLE) && defined(CONFIG_VGA_ENABLE) && defined(__AVR_ATmega328P__)

#ifndef DEJITTER_OFFSET
#define DEJITTER_OFFSET -4
#endif

#if defined(CONFIG_VGA_128X64_ENABLE)

static const uint16_t __VGA_VERTICAL_PIXELS = 64;
static const uint16_t __VGA_LINE_BYTES = 16;
static const uint16_t __VGA_PIXEL_HEIGHT = 7;
volatile uint8_t      __vga_buffer[16*64] = {0};
static const volatile uint8_t *__VGA_BUFFER_PTR = &__vga_buffer[0];

void ssd1306_vga_controller_128x64_init_no_output(void);
void ssd1306_vga_controller_128x64_init_enable_output(void);
void ssd1306_vga_controller_128x64_init_enable_output_no_jitter_fix(void);
void ssd1306_debug_print_vga_buffer_128x64(void (*func)(uint8_t));

#elif defined(CONFIG_VGA_96X40_ENABLE)

static const uint16_t __VGA_VERTICAL_PIXELS = 40;
static const uint16_t __VGA_LINE_BYTES = 36;
static const uint16_t __VGA_PIXEL_HEIGHT = 10;
volatile uint8_t      __vga_buffer[36*40] = {0};
static const volatile uint8_t * __VGA_BUFFER_PTR = &__vga_buffer[0];

void ssd1306_vga_controller_96x40_init_no_output(void);
void ssd1306_vga_controller_96x40_init_enable_output(void);
void ssd1306_vga_controller_96x40_init_enable_output_no_jitter_fix(void);
void ssd1306_debug_print_vga_buffer_96x40(void (*func)(uint8_t));

#else
    #error "Please, define one of VGA options"
#endif

#if !defined(VGA_CONTROLLER_DEBUG)

// Total number of lines used in specific scan mode
static const uint16_t VGA_TOTAL_MODE_LINES = __VGA_VERTICAL_PIXELS*__VGA_PIXEL_HEIGHT;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
static const uint8_t V_BACKPORCH_LINES = 40;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
volatile int s_current_scan_line;

volatile uint8_t s_lines_to_skip;
volatile uint8_t s_scan_line_index;
volatile const uint8_t * volatile s_current_scan_line_data = __VGA_BUFFER_PTR;
extern volatile uint8_t s_vga_frames;
extern unsigned long timer0_millis;
// ISR: Vsync pulse
ISR(TIMER1_OVF_vect)
{
    s_current_scan_line = 0;
    s_scan_line_index = 0;
    s_current_scan_line_data = __VGA_BUFFER_PTR;
    s_lines_to_skip = V_BACKPORCH_LINES;
    s_vga_frames++;
    timer0_millis += 16;
} // end of TIMER1_OVF_vect

#if defined(CONFIG_VGA_128X64_ENABLE)
static inline void /*__attribute__ ((noinline))*/ do_scan_line()
{
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
}
#elif defined(CONFIG_VGA_96X40_ENABLE)
static inline void /*__attribute__ ((noinline))*/ do_scan_line()
{
    // output all pixels

    asm volatile(
         ".rept 12\n\t"

         "ld r24, Z+\n\t"        // r24 = 82227111
//         "nop\n\t"               // to make pixel wider in 96x40 mode
         "out %[port], r24\n\t"  // 111

         "ld r25, Z+\n\t"        // r25 = 84447333
         "swap r24\n\t"          // r24 = 71118222
         "out %[port], r24\n\t"  // 222

         "andi r24, 0x88\n\t"    // r24 = 70008000
         "ld r20, Z+\n\t"        // r20 = 86667555
         "out %[port], r25\n\t"  // 333

         "swap r25\n\t"          // r25 = 73338444
         "lsr r24\n\t"           // r24 = 00080007
         "lsr r24\n\t"           // r24 = 00800070
         "out %[port], r25\n\t"  // 444

         "andi r25, 0x88\n\t"    // r25 = 70008000
         "lsr r24\n\t"           // r24 = 08000700
         "lsr r25\n\t"           // r25 = 00080007
         "out %[port], r20\n\t"  // 555

         "swap r20\n\t"          // r20 = 75558666
         "lsr r25\n\t"           // r25 = 00800070
         "or r24, r25\n\t"       // r24 = 08800770
         "out %[port], r20\n\t"  // 666

         "andi r20, 0x88\n\t"    // r20 = 70008000
         "lsr r20\n\t"           // r20 = 00080007 b
         "or r24, r20\n\t"       // r24 = 08880777 rgb
         "out %[port], r24\n\t"  // 777

         "swap r24\n\t"          // r24 = 07770888
//         "nop\n\t"               // to make pixel wider in 96x40 mode
         "nop\n\t"
         "out %[port], r24\n\t"  // 888

         ".endr\n\t"
//         "nop\n\t"               // to make pixel wider in 96x40 mode
         "nop\n\t"
         "ldi r24,0\n\t"
         "out %[port], r24 \n\t"
         "nop\n\t"
    :
    : [port] "I" (_SFR_IO_ADDR(PORTC)),
       "z" "I" ((uint8_t *)s_current_scan_line_data )
    : "r24", "r25", "r20", "memory"
    );
}
#endif

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
    else if (s_current_scan_line >= VGA_TOTAL_MODE_LINES)
    {
        return;
    }
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
    do_scan_line();
    s_current_scan_line++;
    s_scan_line_index++;
    if ( s_scan_line_index >= __VGA_PIXEL_HEIGHT )
    {
        s_scan_line_index=0;
        s_current_scan_line_data += __VGA_LINE_BYTES;
    }
}

#endif  // VGA_CONTROLLER_DEBUG

#if defined(CONFIG_VGA_128X64_ENABLE)

static inline void ssd1306_vga_controller_init(void)
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

void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t))
{
    ssd1306_debug_print_vga_buffer_128x64(func);
}

#elif defined(CONFIG_VGA_96X40_ENABLE)

static inline void ssd1306_vga_controller_init(void)
{
    // if there is no builtin support then only debug mode is available
#if defined(VGA_CONTROLLER_DEBUG)
    ssd1306_vga_controller_96x40_init_no_output();
#elif defined(SSD1306_VGA_SLEEP_MODE)
    ssd1306_vga_controller_96x40_init_enable_output_no_jitter_fix();
#else
    ssd1306_vga_controller_96x40_init_enable_output();
#endif
}

void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t))
{
    ssd1306_debug_print_vga_buffer_96x40(func);
}

#endif // CONFIG_VGA_XXX_ENABLE

#endif  // SSD1306_BUILTIN_VGA_SUPPORT

#ifdef __cplusplus
}
#endif

#endif

