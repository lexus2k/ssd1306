/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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
 * @file ssd1306_console.h This is for console
 */

#ifndef _SSD1306_CONSOLE_H_
#define _SSD1306_CONSOLE_H_

#include "ssd1306.h"
#include "ssd1306_hal/io.h"
#include "ssd1306_hal/Print_internal.h"

/**
 * Callback function to print text to the LCD display
 */
typedef size_t (*LcdWriter)(uint8_t ch);

/**
 * Base template class for specific LCD console implementation
 */
template <LcdWriter W>
class LcdConsole: public Print
{
public:
    /**
     * Creates console object to print text information on LCD display.
     */
    explicit LcdConsole( ) { };

    /**
     * Initializes console.
     */
    void   begin()
    {
    }

    /**
     * Writes single character to the display
     * @param ch - character to write
     */
    size_t write(uint8_t ch) override
    {
        return W(ch);
    }

private:

};

/**
 * ssd1306 console support function.
 * @param ch character to print
 * @warning only for SSD1306 based displays
 */
size_t ssd1306_consoleWriter(uint8_t ch);

/**
 * Ssd1306Console represents object to work with LCD display.
 * Easy to use:
 * ~~~~~~~~~~~~~~~{.cpp}
 * Ssd1306Console  console;
 * void setup()
 * {
 *      ssd1306_128x64_spi_init(3, 4, 5);
 *      ssd1306_clearScreen();
 *      console.print( "Hello" );
 * }
 * ~~~~~~~~~~~~~~~
 */
class Ssd1306Console: public LcdConsole<ssd1306_consoleWriter>
{
public:
    using LcdConsole::LcdConsole;

    /**
     * Fills screen with zero-byte and sets
     * cursor position to top-left corner of the screen.
     */
    void   clear();

    /**
     * Set cursor position for text functions
     *
     * @param x horizontal position in pixels.
     * @param y vertical position in pixels.
     */
    void   setCursor(lcduint_t x, lcduint_t y);

private:

};

#endif

