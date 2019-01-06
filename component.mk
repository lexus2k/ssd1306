# This is Makefile for ESP32 IDF

COMPONENT_ADD_INCLUDEDIRS := ./src
COMPONENT_SRCDIRS := ./src \
                     ./src/ssd1306_hal \
                     ./src/ssd1306_hal/esp \
                     ./src/intf \
                     ./src/intf/i2c \
                     ./src/intf/spi \
                     ./sec/intf/vga/esp32 \
                     ./sec/intf/vga \
                     ./src/lcd \
                     ./src/nano_engine

