/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Copyright (c) 2015 Neven Boyanov, Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Please, as a favour, retain the link http://tinusaur.org to The Tinusaur Project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */
/**
 * @file font6x8.h Fonts 6x8
 */

#ifndef FONT6x8_H
#define FONT6x8_H

#include "hal/io.h"
#include <stdint.h>

/**
 * @defgroup LCD_FONTS LCD Fonts
 * @{
 */

/** Standard ASCII 6x8 font */
extern const PROGMEM uint8_t ssd1306xled_font6x8 [];

/** Standard ASCII 6x8 AB font with only capital letters */
extern const PROGMEM uint8_t ssd1306xled_font6x8_AB [];

/**
 * @}
 */

#endif // FONT6x8_H
