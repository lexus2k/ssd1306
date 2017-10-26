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

#include "font6x8.h"
#include "ssd1306.h"

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

static uint8_t getMaxScreenItems()
{
    return (ssd1306_displayHeight() >> 3) - 2;
}

void ssd1306_createMenu(SAppMenu *menu, const char **items, uint8_t count)
{
    menu->items = items;
    menu->count = count;
    menu->selection = 0;
    menu->oldSelection = 0;
    menu->scrollPosition = 0;
}

static uint8_t calculateScrollPosition(SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems() - 1)
    {
        return selection - getMaxScreenItems() + 1;
    }
    return menu->scrollPosition;
}

static void drawMenuItem(SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        ssd1306_negativeMode();
    }
    else
    {
        ssd1306_positiveMode();
    }
    ssd1306_charF6x8(8, index - menu->scrollPosition + 1, menu->items[index], STYLE_NORMAL );
    ssd1306_positiveMode();
}

void ssd1306_showMenu(SAppMenu *menu)
{
    ssd1306_drawRect(4, 4, ssd1306_displayWidth() - 5, ssd1306_displayHeight() - 5);
    menu->scrollPosition = calculateScrollPosition( menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, menu->scrollPosition + getMaxScreenItems()); i++)
    {
        drawMenuItem(menu, i);
    }
    menu->oldSelection = menu->selection;
}

void ssd1306_updateMenu(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition( menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            ssd1306_clearScreen();
            ssd1306_showMenu(menu);
        }
        else
        {
            drawMenuItem(menu, menu->oldSelection);
            drawMenuItem(menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

uint8_t ssd1306_menuSelection(SAppMenu *menu)
{
    return menu->selection;
}

void ssd1306_menuDown(SAppMenu *menu)
{
    if (menu->selection < menu->count - 1)
    {
        menu->selection++;
    }
    else
    {
        menu->selection = 0;
    }
}

void ssd1306_menuUp(SAppMenu *menu)
{
    if (menu->selection > 0)
    {
        menu->selection--;
    }
    else
    {
        menu->selection = menu->count - 1;
    }
}
