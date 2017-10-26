# SSD1306 OLED display driver, PCD8544 LED display driver

## Introduction

SSD1306 i2c OLED Display driver is Arduino style library, intended for use with
very small microcontrollers (with a little of SRAM). It was developed to use as
few resources as possible. With internal i2c impementation and without using buffer-related 
functions the library requires only 26 bytes of SRAM. Since ATTiny controllers have no division and multiply
operations, the library uses shift operation to speed up calculations.

## Key Features

 * Internal i2c implementation for Attiny controllers (for Atmega the library uses Wire library).
 * Low level i2c OLED Driver functions (ssd1306_i2c.h) for direct communication with lcd display.
 * Supported OLED displays:
   * SSD1306 128x64
   * SSD1306 128x32
   * PCD8544 84x48 (experimental support)
 * Main API functions are:
   * Primitive graphics functions (line,rectangle,pixels)
   * Printing text to display
   * Drawing bitmap images, located in SRAM or Flash memory (PROGMEM)
   * Work with sprite objects
   * Creating and controlling menu items (see ssd1306_demo example)
 * Memory-buffered operations (nano_gfx.h) for flickering-free output.
 * Bonus games:
   * Arkanoid game as example!
   * Simple Lode runner game as example!

![Image of arkanoid intro](https://github.com/lexus2k/ssd1306/blob/master/examples/arkanoid/screenshots/introscreen.png)

![Image of lode runner](https://github.com/lexus2k/ssd1306/blob/master/examples/lode_runner/screenshots/main_screen.png)

![Image of menu example](https://github.com/lexus2k/ssd1306/blob/master/examples/ssd1306_demo/screenshots/mainmenu_top.png)

For Attiny85/Attiny45 controllers OLED display must be connected to PB3 (SCL) and PB4 (SDA) lines.
For other controllers pins are defined by standard Wire library. The pins can be modified for
Attiny in ssd1306_i2c_conf.h file.

Tested controllers:

 * Attiny85, Attiny45
 * Atmega328p, Atmega168
 * Atmega2560
 * ESP8266 (check [examples compatibility list](examples/ESP8266_compatibility.txt))

## Setting up

 * Download source from https://github.com/lexus2k/ssd1306
 * Put the sources to Arduino/libraries/ folder

For more information about this library, please, visit https://github.com/lexus2k/ssd1306.
Doxygen documentation can be found at [github.io site](http://lexus2k.github.io/ssd1306).
If you found any problem or have any idea, please, report to Issues section.

## License

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

