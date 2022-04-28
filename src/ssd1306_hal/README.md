# Hardware abstraction layer

This directory contain platform specific implementation of hardware abstraction layer.

  * arduino dir: for all Arduino platforms (if you use Arduino IDE)
  * avr dir: for plain avr-gcc environment
  * esp dir: for plain esp8266/esp32 environment
  * linux dir: for linux platforms including raspberry pi
  * mingw dir: for running under windows
  * stm32 dir: for plain stm32 support (not implemented)
  * energia dir: for Energia platforms w/ MSP432P401R

Edit UserSettings.h header file, if you want to disable some parts of ssd1306 library to reduce memory consumption in your project
  
