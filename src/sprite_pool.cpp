/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "sprite_pool.h"
#include "ssd1306.h"


SpritePool::SpritePool( )
   : m_canvas( 8, 8, m_canvasBuf)
   , m_canvasBuf{0}
   , m_sprites{0}
   , m_count( 0 )
{
    m_rect.left = 0;
    m_rect.top = 0;
    m_rect.right = (ssd1306_displayWidth() >> 3) - 1;
    m_rect.bottom = (ssd1306_displayHeight() >> 3) - 1;
};

void SpritePool::drawBlock(uint8_t blockColumn, uint8_t blockRow)
{
    m_canvas.clear();
};

void SpritePool::drawSprites()
{
    for (uint8_t i = 0; i < m_count; i++)
    {
        SPRITE * sprite = m_sprites[i];
        if ( sprite->isNearMove( ) )
        {
            updateRegion(sprite->getUpdateRect());
        }
        else
        {
            updateRegion(sprite->getRect());
            updateRegion(sprite->getLRect());
        }
        sprite->lx = sprite->x;
        sprite->ly = sprite->y;
    }
}

void SpritePool::refreshScreen()
{
    updateRegion( (SSD1306_RECT){ (uint8_t)(m_rect.left<<3),
                                  (uint8_t)(m_rect.top<<3),
                                  (uint8_t)(m_rect.right<<3),
                                  (uint8_t)(m_rect.bottom<<3) } );
}

uint8_t SpritePool::add( SPRITE &sprite )
{
    uint8_t index = m_count;
    if (index >= MAX_SPRITES)
    {
        return SpritePool::SP_ERR_NO_SPACE;
    }
    m_sprites[index] = &sprite;
    m_count++;
    return index;
};

void SpritePool::clear()
{
    m_count = 0;
};

void SpritePool::remove( SPRITE &sprite )
{
    updateRegion( sprite.getLRect() );
    for (uint8_t i=0; i<m_count; i++)
    {
        if (m_sprites[i] == &sprite)
        {
            m_count--;
            for (uint8_t j=i; j<m_count; j++)
            {
                m_sprites[j] = m_sprites[j+1];
            }
            break;
        }
    }
} 


void SpritePool::updateRegion(SSD1306_RECT ur)
{
    ur.left >>= 3;
    ur.top >>= 3;
    ur.right >>= 3;
    ur.bottom >>= 3;
    ur.left = max(ur.left, m_rect.left);
    ur.top = max(ur.top, m_rect.top);
    ur.right = min(ur.right, m_rect.right);
    ur.bottom = min(ur.bottom, m_rect.bottom);
    for(uint8_t x = ur.left; x <= ur.right; x++)
    {
       for(uint8_t y = ur.top; y <= ur.bottom; y++)
       {
           drawBlock(x,y);
           for (uint8_t i = 0; i < m_count; i++)
           {
               m_canvas.drawSpritePgm(m_sprites[i]->x - (x << 3),
                                      m_sprites[i]->y - (y << 3),
                                      m_sprites[i]->data );
           }
           m_canvas.blt( x << 3, y );
       }
    }
}
