/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

#ifndef _SDL_CORE_H_
#define _SDL_CORE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    SDL_LCD_TEMPLATE,
    SDL_LCD_SSD1306,
    SDL_LCD_SH1106,
    SDL_LCD_PCD8544,
    SDL_LCD_SSD1325,
    SDL_LCD_SSD1327,
    SDL_LCD_SSD1331_X8,
    SDL_LCD_SSD1331_X16,
    SDL_LCD_SSD1351,
    SDL_LCD_IL9163,
    SDL_LCD_ST7735,
    SDL_LCD_ILI9341,
};

/** ssd1325 / ssd1327 types */
enum
{
    SDL_LCD_SSD1325_GENERIC,
    SDL_LCD_SSD1327_GENERIC,
    SDL_LCD_SSD1327_NO_COM_SPLIT,
};

extern void sdl_core_init(void);
extern void sdl_core_draw(void);

extern void sdl_set_dc_pin(int pin);
// Accepts pointer to six-elements array
extern void sdl_set_gpio_keys(const uint8_t * pins);
extern void sdl_send_init();
extern void sdl_send_byte(uint8_t data);
extern void sdl_send_stop();
extern int  sdl_read_analog(int pin);
extern void sdl_write_digital(int pin, int value);
extern int sdl_read_digital(int pin);

/** Allocates buffer, returns number of bytes allocated */
extern void sdl_core_get_pixels_data( uint8_t *pixels, uint8_t target_bpp );
/** Returns length in bytes, required to hold the data */
extern int sdl_core_get_pixels_len( uint8_t target_bpp );
extern void sdl_core_set_unittest_mode(void);
extern void sdl_core_close(void);

#ifdef __cplusplus
}
#endif

#endif

