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

#include "game_field.h"

uint8_t gameField[16*7] =
{
   5,0,0,0,0,3,3,0,0,0,0,0,0,0,0,5,
   5,0,2,0,4,0,0,2,1,1,2,0,4,0,0,5,
   5,0,2,0,1,1,0,2,0,0,1,1,1,1,0,5,
   5,0,2,0,0,0,0,2,0,4,0,0,0,0,0,5,
   5,1,2,1,1,1,1,1,1,1,1,1,2,1,1,5,
   5,0,2,0,0,0,0,0,4,0,0,0,2,1,0,5,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,5
};

GameField field;

