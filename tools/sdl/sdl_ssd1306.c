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

#include "sdl_ssd1306.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

#define SSD1306_MAX_BANKS 8

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static uint8_t detected = 0;

static uint8_t gdram[128][64];

static uint8_t displayRemap = 0;
static uint8_t displayStartLine = 0;
static uint8_t displayOffset = 0;
static uint8_t multiplexRatio = 15;
static uint8_t displayOn = 0;

static void blt_single_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint16_t sdl_color = color ? 0xAD59: 0x0000;
    gdram[x & 0x7F ][y & 0x3F] = color;
    uint8_t line = (y - displayOffset - displayStartLine) & 0x3F;
    if ( displayRemap )
    {
        line = multiplexRatio - line;
    }
    if ( line < sdl_ssd1306.height )
    {
        sdl_put_pixel(x, line, sdl_color);
    }
}

static void blt_content()
{
    for(uint8_t line = 0; line < sdl_ssd1306.height; line++)
    {
        uint8_t row = line;
        uint8_t ram = line;
        if ( displayRemap )
        {
            row = multiplexRatio - row;
            ram = multiplexRatio - ram;
        }
        ram = (row + displayOffset + displayStartLine) & 0x3F;
        row = (row + displayOffset) & 0x3F;
        for(uint8_t column = 0; column < sdl_ssd1306.width; column++)
        {
            uint16_t color = (gdram[column][ram]) ? 0xAD59: 0x0000;
            if ( row > (multiplexRatio + displayStartLine) ) color = 0x0000;
            sdl_put_pixel(column, line, color);
        }
    }
}

static void sdl_ssd1306_reset(void)
{
    detected = 0;
}

static int sdl_ssd1306_detect(uint8_t data)
{
    if (detected)
    {
        return 1;
    }
    detected = (data == SDL_LCD_SSD1306) || (data == SDL_LCD_SH1106);
    return 0;
}

static void sdl_ssd1306_commands(uint8_t data)
{
//    printf("%02X (CMD: %02X)\n", data, s_commandId);
    switch (s_commandId)
    {
        case 0x21:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_columnStart = ( data >= sdl_ssd1306.width ? sdl_ssd1306.width - 1: data );
                     s_activeColumn = s_columnStart;
                     break;
                case 1:
                     s_columnEnd = ( data >= sdl_ssd1306.width ? sdl_ssd1306.width - 1: data );
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x22:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_pageStart = (data >= SSD1306_MAX_BANKS ? SSD1306_MAX_BANKS - 1 : data);
                     s_activePage = s_pageStart;
                     break;
                case 1:
                     s_pageEnd = (data >= SSD1306_MAX_BANKS ? SSD1306_MAX_BANKS - 1 : data);
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0xA8:
            if (s_cmdArgIndex == 0)
            {
                multiplexRatio = data;
                sdl_ssd1306.height = data + 1;
                sdl_graphics_set_oled_params(sdl_ssd1306.width,
                                             sdl_ssd1306.height,
                                             sdl_ssd1306.bpp,
                                             sdl_ssd1306.pixfmt);
                if ( displayOn ) blt_content();
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xAF:
            displayOn = 1;
            blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0x81:
            if ( s_cmdArgIndex == 0 )
            {
                // Just skip contrast command
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xD3: // Display offset
            if ( s_cmdArgIndex == 0 )
            {
                displayOffset = data;
                if ( displayOn ) blt_content();
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        default:
            /* Other ssd1306 commands, many commands are combined with data */
            if ((s_commandId >= 0xb0) && (s_commandId <= 0xbf))
            {
                s_activePage =  (uint16_t)(s_commandId & 0x0F);
            }
            if ((s_commandId <= 0x0F))
            {
                s_activeColumn = (s_activeColumn & 0xFFF0) | (uint16_t)s_commandId;
            }
            if ((s_commandId >= 0x40) && (s_commandId <= 0x7F))
            {
                displayStartLine = s_commandId & 0x3F;
                if ( displayOn ) blt_content();
            }
            if ((s_commandId <= 0x1F) && (s_commandId >= 0x10))
            {
                s_activeColumn = (s_activeColumn & 0x000F) | ((int16_t)(s_commandId & 0x0F) << 4);
            }
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}

void sdl_ssd1306_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    for (int i=0; i<8; i++)
    {
        blt_single_pixel( x, (y<<3) + i, data & (1<<i) );
    }
    s_activeColumn++;
    if (s_activeColumn > s_columnEnd)
    {
        s_activeColumn = s_columnStart;
        s_activePage++;
        if (s_activePage > s_pageEnd)
        {
            s_activePage = s_pageStart;
        }
    }
}


sdl_oled_info sdl_ssd1306 =
{
    .width = 128,
    .height = 64,
    .bpp = 16,
    .pixfmt = SDL_PIXELFORMAT_RGB565,
    .dataMode = SDMS_AUTO,
    .detect = sdl_ssd1306_detect,
    .run_cmd = sdl_ssd1306_commands,
    .run_data = sdl_ssd1306_data,
    .reset = sdl_ssd1306_reset,
};
