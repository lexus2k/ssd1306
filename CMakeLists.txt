#    MIT License
#
#    Copyright (c) 2018-2019, Alexey Dynda
#    Copyright (c) 2020, Dana H. Myers
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.
#

idf_component_register(SRCS "src/intf/i2c/ssd1306_i2c.c"
    "src/intf/i2c/ssd1306_i2c_embedded.c"
    "src/intf/i2c/ssd1306_i2c_twi.c"
    "src/intf/spi/ssd1306_spi.c"
    "src/intf/spi/ssd1306_spi_avr.c"
    "src/intf/spi/ssd1306_spi_usi.c"
    "src/intf/ssd1306_interface.c"
    "src/intf/uart/ssd1306_uart_builtin.c"
    "src/intf/vga/atmega328p/vga128x64.c"
    "src/intf/vga/atmega328p/vga96x40.c"
    "src/intf/vga/esp32/CompositeOutput.cpp"
    "src/intf/vga/esp32/vga128x64.cpp"
    "src/intf/vga/vga.c"
    "src/lcd/composite_video.c"
    "src/lcd/lcd_common.c"
    "src/lcd/lcd_il9163.c"
    "src/lcd/lcd_ili9341.c"
    "src/lcd/lcd_pcd8544.c"
    "src/lcd/oled_sh1106.c"
    "src/lcd/oled_ssd1306.c"
    "src/lcd/oled_ssd1325.c"
    "src/lcd/oled_ssd1327.c"
    "src/lcd/oled_ssd1331.c"
    "src/lcd/oled_ssd1351.c"
    "src/lcd/oled_template.c"
    "src/lcd/vga_monitor.c"
    "src/nano_engine/canvas.cpp"
    "src/nano_engine/core.cpp"
    "src/nano_gfx.cpp"
    "src/sprite_pool.cpp"
    "src/ssd1306_16bit.c"
    "src/ssd1306_1bit.c"
    "src/ssd1306_8bit.c"
    "src/ssd1306_console.cpp"
    "src/ssd1306_fonts.c"
    "src/ssd1306_generic.c"
    "src/ssd1306_hal/arduino/platform.cpp"
    "src/ssd1306_hal/avr/platform.c"
    "src/ssd1306_hal/esp/platform.c"
    "src/ssd1306_hal/linux/platform.c"
    "src/ssd1306_hal/mingw/platform.c"
    "src/ssd1306_hal/stm32/platform.c"
    "src/ssd1306_hal/template/platform.c"
    "src/ssd1306_menu.c"
    "src/ssd1306_uart.c"
  INCLUDE_DIRS "src")

