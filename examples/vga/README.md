# VGA SUPPORT

## Introduction

Vga support have 2 modes:

 * builtin directly to your sketch
 * client mode

In the first case the atmega328p controller executes sketch application and drives vga monitor. The code uses
all 3-timers, so standard micros() and delay() functions do not work in this mode! millis() still work, but
it looses precision. The code needs 3 timers for the following reasons: *timer0* to fix jitter effects,
*timer1* to drive V-Sync signal, and *timer2* to drive H-Sync signal. If you want to use delay(), please call
ssd1306_vga_delay() instead.

In the client mode, the controller executes sketch application only, so, all standard functions are available.
The ssd1306 library part needs only UART to send commands to the second controller, which runs vga_server_demo.

## Supported controllers

 * Only Atmega328p is supported now in server and built-in modes
 * Any controller can be supported in client mode.

## Connecting Atmega328p to D-Sub

There are a lot of articles on D-Sub pinout over Internet. You need to connect GND, V-Sync, H-Sync, R, G, B
to D-Sub connector via resistors. ssd1306 library for Atmega328p implementation uses the following pins:

 * D3    - H-SYNC
 * D10   - V-SYNC
 * D14 (A0) - BLUE
 * D15 (A1) - GREEN
 * D16 (A2) - RED
 * D17/D18/D19 cannot be used for your sketch
 