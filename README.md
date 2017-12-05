# SSD1306 OLED display driver, PCD8544 LED display driver

## Introduction

SSD1306 OLED display driver is Arduino style library, intended for use with
very small microcontrollers (with a little of SRAM). It was developed to use as
few resources as possible. Since ATTiny controllers have no division and multiply
operations, the library uses shift operation to speed up calculations.

## Key Features

 * Embedded i2c software implementation
 * Standard i2c support (via Wire library)
 * 4-wire spi support (via standard SPI library)
 * Usage of very little of SRAM:
   * Attiny85 with Damellis package (minimum 25 bytes of SRAM)
   * Atmega328p (minimum 56 bytes of SRAM)
   * ESP8266 (minimum 26 bytes of SRAM)
 * The library uses as little Flash memory as possible
 * Fast implementation to provide reasonable speed on slow microcontrollers
 * Primitive graphics functions (line,rectangle,pixels)
 * Printing text to display (using 6x8 fonts, and double size output 12x16)
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

The default i2c pins for embedded implementation can be modified in ssd1306_i2c_conf.h file.
For other controllers pins are defined by standard Wire library.
The default spi SCLK and MOSI pins are defined by SPI library, and DC, RST, CES pins are
configurable through API.

## Supported displays:
 * i2c ssd1306 128x64 oled display
 * i2c ssd1306 128x32 oled display
 * spi ssd1306 128x64 oled display
 * spi pcd8544 84x48 nokia 5110 led display (experimental support)

## Supported platforms

 * Attiny85, Attiny45 (Refer to [Damellis attiny package](https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json))
 * Atmega328p, Atmega168
 * Atmega2560
 * Digispark, including PRO version (check [examples compatibility list](examples/Digispark_compatibility.txt))
 * ESP8266 (check [examples compatibility list](examples/ESP8266_compatibility.txt))
 * ESP32 Dev Board (check [examples compatibility list](examples/ESP8266_compatibility.txt))

Digispark users, please check compilation options in your Arduino prior to using this library.
Ssd1306 library requires at least c++11 and c99 (by default Digispark package misses the options
-std=gnu11, -std=gnu++11).

## The goals of ssd1306 library

 * To use as few RAM as possible
 * To use as few Flash as possible
 * To be as fast as possible
 * To fit [Arkanoid game example](examples/arkanoid) to Attiny85 microcontroller

## Setting up

 * Download source from https://github.com/lexus2k/ssd1306
 * Put the sources to Arduino/libraries/ folder

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


Copyright (C) 2016-2017 Alexey Dynda

This file is part of SSD1306 Library.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

