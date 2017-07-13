/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of SSD1306 library.

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
*/

#pragma once

#include <stdint.h>

const uint8_t BUTTON_NONE   = 0;
const uint8_t BUTTON_RIGHT  = 1;
const uint8_t BUTTON_UP     = 2;
const uint8_t BUTTON_DOWN   = 3;
const uint8_t BUTTON_LEFT   = 4;
const uint8_t BUTTON_SELECT = 5;

uint8_t getPressedButton(uint8_t analogPin);

