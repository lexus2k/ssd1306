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
 * @file ssd1306_hal/Print_internal.h SSD1306 Print class implementation
 */

#ifndef _SSD1306_HAL_PRINT_INTERNAL_H_
#define _SSD1306_HAL_PRINT_INTERNAL_H_

#if defined(ARDUINO) && !defined(ARDUINO_FAKE)

/* Include standard print class for Arduino environment */
#include "Print.h"

#else

#include "ssd1306_hal/io.h"
#include <stdio.h>

/** Implements own Print class for plain AVR and Linux environment */
class Print
{
public:
    /** Constructor to create Print class object */
    Print() {}

    /**
     * abstract function to be defined in inherited classes
     * @param ch char to print
     * @return returns number of printed symbols
     */
    virtual size_t write(uint8_t ch) = 0;

    /**
     * Prints string via write()
     * @param str string to print
     * @return returns number of printed symbols
     */
    size_t print(const char* str)
    {
        size_t n = 0;
        while (*str)
        {
            n += write(*str);
            str++;
        }
        return n;
    }

    /**
     * Prints number via write()
     * @param n integer to print
     * @return returns number of printed symbols
     */
    size_t print(int n)
    {
        char a[10];
        snprintf(a, sizeof(a), "%i", n);
        return print( a );
    }

    /**
     * Prints string via write() and goes to next line.
     * @param str string to print
     * @return returns number of printed symbols
     */
    size_t println(const char* str)
    {
        size_t n = print(str);
        n += write('\n');
        return n;
    };

    /**
     * Prints number via write() and goes to next line.
     * @param data integer to print
     * @return returns number of printed symbols
     */
    size_t println(int data)
    {
        size_t n = print(data);
        n += write('\n');
        return n;
    }
};

#endif

#endif
