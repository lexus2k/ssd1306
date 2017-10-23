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
/**
 * @file pcd8544_commands.h PCD8544 commands definitions
 */

#ifndef _PCD8544_COMMANDS_H_
#define _PCD8544_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** PCD8544 LCD driver commands */
enum ESsd1306Commands
{
    PCD8544_NOP             = 0x00,

// Regular functions
    PCD8544_FUNCTIONSET     = 0x20,
    PCD8544_DISPLAYCONTROL  = 0x08,
    PCD8544_SETYADDR        = 0x40,
    PCD8544_SETXADDR        = 0x80,

// Extended functions
    PCD8544_SETTEMP         = 0x04,
    PCD8544_SETBIAS         = 0x10,
    PCD8544_SETVOP          = 0x80,

};

/** PCD8544 supported function modes. */
enum EPcd8544FunctionMode
{
    PCD8544_POWERDOWN           = 0x04,
    PCD8544_VERTICAL_ADDR_MODE  = 0x02,
    PCD8544_EXTENDEDINSTRUCTION = 0x01,
};

/** PCD8544 supported display modes. */
enum EPcd8544DisplayMode
{
    PCD8544_DISPLAYBLANK        = 0x0,
    PCD8544_DISPLAYALLON        = 0x1,
    PCD8544_DISPLAYNORMAL       = 0x4,
    PCD8544_DISPLAYINVERTED     = 0x5,
};

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _PCD8544_COMMANDS_H_
