# SSD1306/SSD1331 OLED display driver, PCD8544 LED display driver

## Introduction

SSD1306 driver is Arduino style C/C++ library. it can be compiled with plain avr-gcc compiler
without Arduino libraries and supports monochrome and RGB oleds. It is intended for use with
very small microcontrollers (with a little of RAM). It was developed to use as
few resources as possible. Since ATTiny controllers have no division and multiply
operations, the library uses shift operation to speed up calculations.

Do not forget pull-up resistors for i2c lines.

## Key Features

 * The library uses as little Flash memory as possible
 * Supports many interfaces:
   * i2c (software implementation, via Wire library, AVR Twi, Linux i2c-dev)
   * spi (4-wire spi via Arduino SPI library, AVR Spi, AVR USI module)
 * Usage of very little of RAM:
   * Attiny85 with Damellis package (minimum 25 bytes of RAM)
   * Atmega328p (minimum 56 bytes of RAM)
   * ESP8266 (minimum 26 bytes of RAM)
 * Fast implementation to provide reasonable speed on slow microcontrollers
 * Primitive graphics functions (line,rectangle,pixels)
 * Printing text to display (using fonts of different size)
 * Drawing bitmap images, located in RAM or Flash memory (PROGMEM)
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
 * spi pcd8544 84x48 nokia 5110 led display

## Supported platforms

| **Platforms** | **I2C** | **SPI** | **Comments** |
| :-------- |:---:|:---:|:---------|
| **Arduino** |     |     |          |
| Attiny85, Attiny45  |  X  |  X  | Refer to [Damellis attiny package](https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json) |
| Atmega328p, Atmega168  |  X  |  X  |    |
| Atmega2560  |  X  |  X  |    |
| Digispark, including PRO version  |  X  |  X  |  check [examples compatibility list](examples/Digispark_compatibility.txt)  |
| ESP8266  |  X  |  X  | check [examples compatibility list](examples/ESP8266_compatibility.txt)   |
| ESP32  |  X  |  X  | check [examples compatibility list](examples/ESP8266_compatibility.txt)   |
| **Plain AVR** |   |     |          |
| Attiny85, Attiny45 |  X  |  X  |         |
| Atmega328p, Atmega168 |  X  |  X  |         |
| **Linux**  |    |     |          |
| Raspberry Pi |  X  |      | i2c-dev  |
| [SDL Emulation](https://github.com/lexus2k/ssd1306/wiki/How-to-run-emulator-mode) |  X  |  X  | demo code can be run without real OLED HW via SDL library |
| **Windows**  |    |     |          |
| [SDL Emulation](https://github.com/lexus2k/ssd1306/wiki/How-to-run-emulator-mode) |  X  |  X  | demo code can be run without real OLED HW via MinGW32 + SDL library |

Digispark users, please check compilation options in your Arduino prior to using this library.
Ssd1306 library requires at least c++11 and c99 (by default Digispark package misses the options
-std=gnu11, -std=gnu++11).

## The goals of ssd1306 library

 * To use as few RAM as possible
 * To use as few Flash as possible
 * To be as fast as possible
 * To fit [Arkanoid game example](examples/arkanoid) to Attiny85 microcontroller

## Setting up

*Hardware setup is described [here](https://github.com/lexus2k/ssd1306/wiki/Hardware-setup)

*Setting up for Arduino: (variant 1)*
 * Download source from https://github.com/lexus2k/ssd1306
 * Put the sources to Arduino/libraries/ folder

*Setting up for Arduino: (variant 2)*
 * Install ssd1306 library via Arduino IDE library manager

*Using with plain avr-gcc:*
 * Download source from https://github.com/lexus2k/ssd1306
 * Build the library (variant 1)
   * cd ssd1306/src && make -f Makefile.avr MCU=<your_mcu>
   * Link library to your project (refer to [Makefile.avr](examples/Makefile.avr) in examples folder).
 * Build demo code (variant 2)
   * cd ssd1306/tools && ./build_and_run.sh -p avr -m <your_mcu> ssd1306_demo

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


