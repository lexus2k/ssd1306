# VGA SUPPORT

## Introduction

Vga support have 2 modes:

 * host mode (works directly from your sketch)
 * client mode (vga code works on separate microcontroller)

In the first case the atmega328p controller executes sketch application and drives vga monitor. The vga code uses
all 3-timers, so standard `micros()` and `delay()` functions do not work in this mode! `millis()` still works, but
it looses precision (it updates only each 16 milliseconds). The code needs 3 timers for the following reasons: 
*timer0* to fix jitter effects, *timer1* to drive V-Sync signal, and *timer2* to drive H-Sync signal. If you want
to use `delay()`, please call `ssd1306_vga_delay()` instead.

In client mode, the controller executes sketch application only, so, all standard functions are available. The vga code
runs on separate microcontroller connected with your EVK via UART line. So in client mode, the ssd1306 library part needs
only UART to send commands to the second controller, which runs vga_server_demo; and you are free to use any pins you need,
except UART pins.

## Supported controllers

 * Only Atmega328p is supported now in host and client modes
 * Any controller can be supported in client mode in future.

## Connecting Atmega328p to D-Sub

There are a lot of articles on D-Sub pinout over Internet. You need to connect GND, V-Sync, H-Sync, R, G, B
to D-Sub connector via resistors. ssd1306 library uses the following pins on Atmega328p:

 * D3    - H-SYNC
 * D10   - V-SYNC
 * D14 (A0) - BLUE
 * D15 (A1) - GREEN
 * D16 (A2) - RED
 * D17/D18/D19 cannot be used for your sketch (this is the cost of optimization).
 
