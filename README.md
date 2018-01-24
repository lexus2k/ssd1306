# SSD1306/SSD1331 OLED display driver, PCD8544 LED display driver

## Introduction

SSD1306 driver is Arduino style C/C++ library. it can be compiled with plain avr-gcc compiler
without Arduino libraries and supports monochrome and RGB oleds. It is intended for use with
very small microcontrollers (with a little of SRAM). It was developed to use as
few resources as possible. Since ATTiny controllers have no division and multiply
operations, the library uses shift operation to speed up calculations.

Do not forget pull-up resistors for i2c lines.

## Key Features

 * The library uses as little Flash memory as possible
 * Supports many interfaces:
   * i2c (software implementation, via Wire library, AVR Twi, Linux i2c-dev)
   * spi (4-wire spi via Arduino SPI library)
 * Usage of very little of SRAM:
   * Attiny85 with Damellis package (minimum 25 bytes of SRAM)
   * Atmega328p (minimum 56 bytes of SRAM)
   * ESP8266 (minimum 26 bytes of SRAM)
 * Fast implementation to provide reasonable speed on slow microcontrollers
 * Primitive graphics functions (line,rectangle,pixels)
 * Printing text to display (using fonts of different size)
 * Drawing bitmap images, located in SRAM or Flash memory (PROGMEM)
 * Work with sprite objects
 * Creating and controlling menu items (see ssd1306_demo example)
 * Memory-buffered operations (nano_gfx.h) for flickering-free output.
 * Bonus examples:
   * Arkanoid game
   * Simple Lode runner game
   * Snowflakes

![Image of arkanoid intro](https://github.com/lexus2k/ssd1306/blob/master/examples/arkanoid/screenshots/introscreen.png)
![Image of lode runner](https://github.com/lexus2k/ssd1306/blob/master/examples/lode_runner/screenshots/main_screen.png)

![Image of menu example](https://github.com/lexus2k/ssd1306/blob/master/examples/ssd1306_demo/screenshots/mainmenu_top.png)
![Image of color oled](https://github.com/lexus2k/ssd1306/blob/master/examples/ssd1331_demo/screenshots/fonts.png)

The default i2c pins for embedded implementation can be modified in ssd1306_i2c_conf.h file.
For other controllers pins are defined by standard Wire library.
The default spi SCLK and MOSI pins are defined by SPI library, and DC, RST, CES pins are
configurable through API.

## Supported displays:
 * i2c sh1106 128x64 oled display
 * i2c ssd1306 128x64 oled display
 * i2c ssd1306 128x32 oled display
 * spi ssd1306 128x64 oled display
 * spi ssd1331 96x64 RGB oled display
 * spi pcd8544 84x48 nokia 5110 led display (experimental support)

## Supported platforms

 * Arduino
   * Attiny85, Attiny45 (Refer to [Damellis attiny package](https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json))
   * Atmega328p, Atmega168
   * Atmega2560
   * Digispark, including PRO version (check [examples compatibility list](examples/Digispark_compatibility.txt))
   * ESP8266 (check [examples compatibility list](examples/ESP8266_compatibility.txt))
   * ESP32 Dev Board (check [examples compatibility list](examples/ESP8266_compatibility.txt))
 * AVR (ssd1306 library can be compiled and used with plain avr-gcc/avr-libc without Arduino libraries)
   * Attiny85, Attiny45
   * Atmega328p, Atmega168
 * Linux
   * Raspberry Pi (i2c-dev)

Digispark users, please check compilation options in your Arduino prior to using this library.
Ssd1306 library requires at least c++11 and c99 (by default Digispark package misses the options
-std=gnu11, -std=gnu++11).

## The goals of ssd1306 library

 * To use as few RAM as possible
 * To use as few Flash as possible
 * To be as fast as possible
 * To fit [Arkanoid game example](examples/arkanoid) to Attiny85 microcontroller

## Setting up

*Setting up for Arduino: (variant 1)*
 * Download source from https://github.com/lexus2k/ssd1306
 * Put the sources to Arduino/libraries/ folder

*Setting up for Arduino: (variant 2)*
 * Install ssd1306 library via Arduino IDE library manager

*Using with plain avr-gcc:*
 * Download source from https://github.com/lexus2k/ssd1306
 * Build the library: cd ssd1306/src && make -f Makefile.avr MCU=<your_mcu>
 * Link library to your project (refer to [Makefile.avr](examples/Makefile.avr) in examples folder).



## Adding new interface

If you need to add support for new interface to the library, just implement
interface specific functions and assign them to function pointers:

```cpp
#include "intf/ssd1306_interface.h"

void setup()
{
    ssd1306_startTransmission = &myStartTransmissionFunc;
    ssd1306_endTransmission   = &myEndTransmissionFunc;
    ssd1306_sendByte          = &mySendByteToInterfaceFunc;
    ssd1306_commandStart      = &myStartCommandModeTransmissionFunc;
    ssd1306_dataStart         = &myStartDataModeTransmissionFunc;
    ...
    # init your interface here
    myInterfaceInit();
    ssd1306_128x64_init();
}
```
Refer to SPI, I2C interfaces init function to understand required implementation.

## Adding new LCD type to the library

The main requirements for LCD display to use it with this library are:

 * Each byte, sent to LCD, represents 8 vertical pixels
 * After sending each byte LCD controller shifts GDRAM pointer to the right by 1 pixel
 * LCD width should not exceed 128 pixels

To add new LCD type to the library, implement GDRAM address positioning functions and
initialization function for you LCD:

```cpp
#include "lcd/lcd_common.h"
#include "intf/ssd1306_interface.h"

void    myDisplayInit()
{
    # Use LCD_TYPE_CUSTOM constant
    g_lcd_type = LCD_TYPE_CUSTOM;
    # Set display size:
    #     Width should be less or equal to 128
    s_displayWidth = 128;
    #     Height should be less or equal to 128
    s_displayHeight = 64;
    ssd1306_setRamBlock = &mySetRamBlockFunc;
    ssd1306_nextRamPage = &myNextPageFunc;
    ssd1306_setRamPos = mySetRamAddressFunc;
    # Put you LCD initialization code here
    ...
}

void setup()
{
    # Init the interface here
    ...
    # Call LCD initialization here
    myDisplayInit();
}
```
Refer to SH1106 intialization implementation as reference code.

For more information about this library, please, visit https://github.com/lexus2k/ssd1306.
Doxygen documentation can be found at [github.io site](http://lexus2k.github.io/ssd1306).
If you found any problem or have any idea, please, report to Issues section.

## License

The library is free. If this project helps you, you can give me a cup of coffee.
[![Donate via Paypal](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/lexus2k)


MIT License

Copyright (c) 2016-2018, Alexey Dynda

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


