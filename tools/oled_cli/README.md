# OLED CLI

## Introduction

oled_cli tool allows to use oled display, connected to raspberry pi, directly from command line.

## Compilation

compile oled_cli tool on your raspberry pi with the command
> make

## Running

example of running oled_cli tool for i2c display
> sudo modprobe i2c-dev<br>
> ./oled_cli i2c 1 0x3c ssd1306_128x64

now you're able to send commands to oled display from command line

> rect 10,10,20,30

