= SSD1306 Driver for ATTiny controllers =

SSD1306 I2C OLED Display library is intended for use on controllers
with very small amount of SRAM. First of all, it is developed for ATTiny
controllers to use as few resources as possible (in simple case it requires
only 20 bytes of SRAM). Since ATTiny controllers have no division and multiply
operations, the library has some limited functionaly, and uses shift operation
to speed up calculations. But it is still can be used to develop simple graphics
applications (please, refer to examples).

Key Features:

 * Fast I2C implementation (ssd1306_i2c.h): for Attiny it has its own I2C implementation, for Atmega it uses Wire library.
 * Low level I2C OLED Driver functions (ssd1306_interface.h)
 * High level I2C OLED Driver functions (ssd1306.h)
 * Memory-buffered operations (nano_gfx.h)
 * Third-party font (6x8)
 * Some functions are:
   * Clearing display
   * Filling display with pattern data
   * Printing text to display
   * Drawing bitmap image from SRAM memory
   * Drawing bitmap image from Flash memory (PROGMEM related)
   * Sprites: moving
   * Drawing lines in buffer (vertical and horizontal)
 * And bonus: Arkanoid game as example!

![Image of arkanoid intro](https://github.com/lexus2k/ssd1306/blob/master/examples/arkanoid/screenshots/introscreen.png) ![Image of arkanoid screen](https://github.com/lexus2k/ssd1306/blob/master/examples/arkanoid/screenshots/screenshot.png)

For Attiny85/Attiny45 controllers OLED display must be connected to
PB3 (SCL) and PB4 (SDA) lines. For other controllers pins
are defined by standard Wire library. The pins
can be modified for Attiny in ssd1306_i2c_pins.h file.

Tested controllers:
* Attiny85, Attiny45
* Atmega328p, Atmega168

For more information about this library please visit
https://github.com/lexus2k/ssd1306.
If you found any problem, or have any idea, please report to Issues section.

== License ==

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

