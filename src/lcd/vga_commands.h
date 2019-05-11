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
 * @file vga_commands.h VGA library commands definitions
 */

#ifndef _VGA_COMMANDS_H_
#define _VGA_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** VGA driver commands */
enum EVgaCommands
{
    /**
     * VGA_SET_BLOCK command sets rectangle in VGA ram to send pixels to.
     * The command needs 4 byte-arguments:
     *    left boundary in pixels,
     *    right boundary in pixels,
     *    top boundary in pixels,
     *    bottom boundary in pixels, (last arg in not implemented yet)
     */
    VGA_SET_BLOCK       = 0x01,

    /**
     * VGA_SET_MODE command sets memory addressing mode: there are 2 modes
     * available: 0 - normal addressing mode, 1 - ssd1306 compatible mode.
     * After sending each pixel in normal addressing mode, x position shifts
     * right by 1 pixel until end of block is reached, then y position shifts
     * down by 1 pixel, and x position move to the start of block.
     * After sending each pixel in ssd1306 compatible mode, y position shifts
     * down by 1 pixel until 8 vertical pixels are printed, then y position changes
     * to top of block, and x position shifts right by 1 pixel.
     */
    VGA_SET_MODE        = 0x02,

    VGA_SET_RESOLUTION  = 0x03,

    VGA_DISPLAY_ON      = 0x04,
};

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif
