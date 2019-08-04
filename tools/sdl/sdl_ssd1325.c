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

#include "sdl_ssd1325.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

static int s_activeColumn = 0;
static int s_activeRow = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_rowStart = 0;
static int s_rowEnd = 7;
static int s_newColumn;
static int s_newPage;
static uint8_t detected = 0;
static uint8_t s_subtype = SDL_LCD_SSD1325_GENERIC;
static uint8_t s_hwComSplit = 0;


static void copyBlock()
{
    if ( s_newColumn < s_columnStart )
    {
        for( int y = s_rowStart; y <= s_rowEnd; y++)
        for( int x = s_newColumn; x <= s_newColumn + s_columnEnd - s_columnStart; x++)
            sdl_put_pixel(x, y, sdl_get_pixel( x + s_columnStart - s_newColumn, y ));
    }
    else
    {
        for( int y = s_rowStart; y <= s_rowEnd; y++)
        for( int x = s_newColumn + s_columnEnd - s_columnStart; x >= s_newColumn; x--)
            sdl_put_pixel(x, y, sdl_get_pixel( x + s_columnStart - s_newColumn, y ));
    }
}

static int sdl_ssd1325_detect(uint8_t data)
{
    if (detected)
    {
        switch (data)
        {
            case SDL_LCD_SSD1325_GENERIC:
                s_hwComSplit = 1;
                sdl_ssd1325.height = 64;
                break;
            case SDL_LCD_SSD1327_GENERIC:
                s_hwComSplit = 1;
                sdl_ssd1325.height = 128;
                break;
            case SDL_LCD_SSD1327_NO_COM_SPLIT:
                s_hwComSplit = 0;
                sdl_ssd1325.height = 128;
                break;
            default:
                break;
        }
        s_subtype = data;
        return 1;
    }
    detected = (data == SDL_LCD_SSD1325 || data == SDL_LCD_SSD1327);
    return 0;
}

static uint8_t s_verticalMode = 1;
static uint8_t s_nimbleMapping = 0;
static uint8_t s_leftToRight = 0;
static uint8_t s_topToBottom = 0;
static uint8_t s_comSplit = 0;

static void sdl_ssd1325_commands(uint8_t data)
{
    switch (s_commandId)
    {
        case 0xA0:
            if (s_cmdArgIndex == 0)
            {
                // A[0] = Enable column address re-map
                // A[1] = Enable Nibble remap
                // A[2] = Enable Vertical Address increment
                // A[4] = Enable COM-remap
                // A[6] = Splitting ODD / Even signals
                s_verticalMode = data & 0x04;
                s_nimbleMapping = data & 0x02;
                s_leftToRight = data & 0x01;
                s_topToBottom = data & 0x10;
                s_comSplit = data & 0x40;
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0x15:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_columnStart = data * 2;
                     s_activeColumn = data * 2;
                     break;
                case 1:
                     s_columnEnd = data * 2 + 1;
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x23: // MOVE BLOCK
            switch (s_cmdArgIndex)
            {
                case 0: s_columnStart = data * 2; break;
                case 1: s_rowStart = data; break;
                case 2: s_columnEnd = data * 2 + 1; break;
                case 3: s_rowEnd = data; break;
                case 4: s_newColumn = data * 2; break;
                case 5:
                     s_newPage = data;
                     copyBlock();
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default:
                     break;
            }
            break;
        case 0x75:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_activeRow = data;
                     s_rowStart = data;
                     break;
                case 1:
                     s_rowEnd = data;
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0xA8: // Multiplex
            if ( s_cmdArgIndex == 0 )
            {
                s_columnEnd = data + 1;
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xAE: // OFF                         /* display off */
        case 0xA4: // NORMAL
        case 0x2E: // Deactivate scroll
        case 0xAF: // Display ON
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xFD: // Unlock OLED
        case 0xA1: // Start line
        case 0xA2: // Display offset
        case 0xAB: // VDD internal
        case 0x81: // CONTRAST
        case 0xB1: // PHASE 0x51
        case 0xB3: // CLK
        case 0xBC: // PRECHARGE
        case 0xBE: // VCOMH voltage
        case 0xB6: // Second pre-charge
            if (s_cmdArgIndex == 0) s_commandId = SSD_COMMAND_NONE;
            break;
        default:
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}


static void sdl_ssd1325_data(uint8_t data)
{
    for (int i=0; i<2; i++)
    {
        int y = s_topToBottom ? s_activeRow : (sdl_ssd1325.height - s_activeRow - 1);
        int x = s_leftToRight ? s_activeColumn: (sdl_ssd1325.width - s_activeColumn - 1);
        if ( (s_comSplit && !s_hwComSplit) || (!s_comSplit && s_hwComSplit) )
        {
            y = ((y & 0x3F) * 2) + ((y > 0x3F) ? 1: 0);
        }
        sdl_put_pixel(x, y, ((data & 0x0F) << 28) |
                            ((data & 0x0F) << 20) |
                            ((data & 0x0F) << 12) |
                            ((data & 0x0F) << 4) );
        if (s_verticalMode)
        {
            if (i == 1)
            {
                s_activeColumn--;
                s_activeRow++;
                if (s_activeRow > s_rowEnd)
                {
                    s_activeRow = s_rowStart;
                    s_activeColumn += 2;
                }
            }
            if (i == 0)
            {
                s_activeColumn++;
            }
            if (s_activeColumn > s_columnEnd)
            {
                s_activeColumn = s_columnStart;
            }
        }
        else
        {
            s_activeColumn++;
            if (s_activeColumn > s_columnEnd)
            {
                s_activeColumn = s_columnStart;
                s_activeRow++;
                if (s_activeRow > s_rowEnd)
                {
                    s_activeRow = s_rowStart;
                }
            }
        }
        data >>= 4;
    }
}

sdl_oled_info sdl_ssd1325 =
{
    .width = 128,
    .height = 64,
    .bpp = 32,
    .pixfmt = SDL_PIXELFORMAT_RGBX8888,
    .dataMode = SDMS_AUTO,
    .detect = sdl_ssd1325_detect,
    .run_cmd = sdl_ssd1325_commands,
    .run_data = sdl_ssd1325_data,
};
