= SSD1306 Driver for ATTiny controllers =

SSD1306 is library intended to work with SSD1306 I2C OLED
Displays. First of all, it is developed for ATTiny controllers
to use as few resources as possible, because of limitation
to SRAM (512 bytes) and Flash (8K). Since ATTiny controller
has no division and multiply operations, the library has
some limited functionaly. But it is still can be used to
develop simple graphics applications.

Key Features:

 * Low level I2C OLED Driver functions (ssd1306_i2c.h)
 * High level I2C OLED Driver functions (ssd1306.h)
 * Some memory-buffered operations (nano_gfx.h)
 * Third-party font (6x8)
 * Some functions are:
 ** Filling display with pattern data
 ** Printing text to display
 ** Drawing bitmap image from SRAM memory
 ** Drawing bitmap image from Flash memory (PROGMEM related)
 ** Sprites: moving
 ** Drawing lines in buffer

For Attiny controllers OLED display must be connected to
PB3 (SCL) and PB4 (SDA) lines. For other controllers connections
are programmed to A5 (SCL) and A4 (SDA) lines. The pins
can be modified in ssd1306_pins.h file.

For more information about this library please visit
https://github.com/lexus2k/ssd1306.
If you found any problem, please report to Issues section.

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

