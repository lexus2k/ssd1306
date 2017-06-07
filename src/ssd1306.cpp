/*
    Copyright (C) 2016-2017 Alexey Dynda

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

void    ssd1306_init()
{
  ssd1306_i2cStop(); // i2c.begin(); begin does the same as stop

  ssd1306_i2cSendCommand(SSD1306_DISPLAYOFF); // display off

  /* actually 2 commands below can be omitted */
  ssd1306_i2cSendCommand(SSD1306_SETLOWCOLUMN); 
  ssd1306_i2cSendCommand(SSD1306_SETHIGHCOLUMN); 
  /* normal page addressing mode */
  ssd1306_i2cSendCommand(SSD1306_SETSTARTLINE);

  ssd1306_i2cSendCommand(SSD1306_SETCONTRAST);
  ssd1306_i2cSendCommand(0x7F); // 0xCF

  ssd1306_i2cSendCommand(SSD1306_SEGREMAP | 0x01);
  ssd1306_i2cSendCommand(SSD1306_COMSCANDEC);
  ssd1306_i2cSendCommand(SSD1306_NORMALDISPLAY);

  ssd1306_i2cSendCommand(SSD1306_SETMULTIPLEX);
  ssd1306_i2cSendCommand(63);

  ssd1306_i2cSendCommand(SSD1306_SETDISPLAYOFFSET);
  ssd1306_i2cSendCommand(0x00); // --no offset

  ssd1306_i2cSendCommand(SSD1306_SETDISPLAYCLOCKDIV);
  ssd1306_i2cSendCommand(0x80);

  ssd1306_i2cSendCommand(SSD1306_SETPRECHARGE);
  ssd1306_i2cSendCommand(0xF1); // --switch vcc

  ssd1306_i2cSendCommand(SSD1306_SETCOMPINS);
  ssd1306_i2cSendCommand(0x12); // --set divide ratio

  ssd1306_i2cSendCommand(SSD1306_SETVCOMDETECT); // --vcom detect
  ssd1306_i2cSendCommand(0x40); // 

  ssd1306_i2cSendCommand(SSD1306_MEMORYMODE);
  ssd1306_i2cSendCommand(0x02); // Page Addressing mode

  ssd1306_i2cSendCommand(SSD1306_CHARGEPUMP);
  ssd1306_i2cSendCommand(0x14);
  
  ssd1306_i2cSendCommand(SSD1306_DISPLAYALLON_RESUME);

  ssd1306_i2cSendCommand(SSD1306_NORMALDISPLAY);

  ssd1306_i2cSendCommand(SSD1306_DISPLAYON); 
}


void ssd1306_setPos(uint8_t x, uint8_t y)
{
  ssd1306_i2cStart();
  ssd1306_i2cSendByte(0x00);	//write command
  ssd1306_i2cSendByte(0xb0+y);
  ssd1306_i2cSendByte((x>>4) | 0x10);
  ssd1306_i2cSendByte(x&0x0f);

  ssd1306_i2cStop();
}

void ssd1306_fillScreen(uint8_t fill_Data){
  uint8_t m,n;
  for(m=0;m<8;m++)
    {
      ssd1306_i2cSendCommand(0xb0+m);	//page0-page1
      ssd1306_i2cSendCommand(0x00);		//low column start address
      ssd1306_i2cSendCommand(0x10);		//high column start address
      ssd1306_i2cDataStart();
      for(n=0;n<128;n++)
        {
          ssd1306_i2cSendByte(fill_Data);
        }
      ssd1306_i2cStop();
    }
}
void ssd1306_clearScreen(){

  for(uint8_t m=0;m<8;m++){
    ssd1306_i2cSendCommand(0xb0+m);	//page0-page1
    ssd1306_i2cSendCommand(0x00);		//low column start address
    ssd1306_i2cSendCommand(0x10);		//high column start address
    ssd1306_i2cDataStart();
    for(uint8_t n=0;n<128;n++){
      ssd1306_i2cSendByte(0x00);
    }
    ssd1306_i2cStop();
  }
}


void         ssd1306_displayOff()
{
    ssd1306_i2cSendCommand(SSD1306_DISPLAYOFF);
}


void         ssd1306_displayOn()
{
    ssd1306_i2cSendCommand(SSD1306_DISPLAYON);
}



void ssd1306_charF6x8(uint8_t x, uint8_t y, const char ch[]){
  uint8_t c,i,j=0;
  while(ch[j] != '\0')
    {
      c = ch[j] - 32;
      if(x>126)
        {
          x=0;
          y++;
        }
      ssd1306_setPos(x,y);
      ssd1306_i2cDataStart();
      for(i=0;i<6;i++)
        {
          ssd1306_i2cSendByte(pgm_read_byte(&ssd1306xled_font6x8[c*6+i]));
        }
      ssd1306_i2cStop();
      x += 6;
      j++;
    }
}

void ssd1306_drawCanvas(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf)
{
    uint8_t i, j;
    for(j=0; j<(h >> 3); j++)
    {
        ssd1306_setPos(x,y);
        ssd1306_i2cDataStart();
        for(i=0;i<w;i++)
        {
            ssd1306_i2cSendByte(*buf++);
        }
        ssd1306_i2cStop();
        y++;
    }
}

void ssd1306_drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf)
{
    uint8_t i, j;
    for(j=0; j<(h >> 3); j++)
    {
        ssd1306_setPos(x,y);
        ssd1306_i2cDataStart();
        for(i=w;i>0;i--)
        {
            ssd1306_i2cSendByte(pgm_read_byte(buf++));
        }
        ssd1306_i2cStop();
        y++;
    }
}

void ssd1306_drawSpriteData(uint8_t x, uint8_t y, const uint8_t *sprite)
{
   uint8_t i;
   ssd1306_setPos(x,y);
   ssd1306_i2cDataStart();
   for(i=0;i<8;i++)
   {
       ssd1306_i2cSendByte(pgm_read_byte(&sprite[i]));
   }
   ssd1306_i2cStop();
}


void ssd1306_drawSprite(SPRITE *sprite)
{
    uint8_t posy = sprite->y >> 3;
    uint8_t offsety = sprite->y & 0x7;
    ssd1306_setPos(sprite->x, posy);
    ssd1306_i2cDataStart();
    for (uint8_t i=0; i < sprite->w; i++)
    {
        ssd1306_i2cSendByte( pgm_read_byte( &sprite->data[i] ) << offsety );
    }
    ssd1306_i2cStop();
    if (offsety)
    {
        ssd1306_setPos(sprite->x, posy + 1);
        ssd1306_i2cDataStart();
        for (uint8_t i=0; i < sprite->w; i++)
        {
            ssd1306_i2cSendByte( pgm_read_byte( &sprite->data[i] ) >> (8 - offsety) );
        }
    }
    ssd1306_i2cStop();
    sprite->lx = sprite->x;
    sprite->ly = sprite->y;
}


void ssd1306_eraseSprite(SPRITE *sprite)
{
    uint8_t posy = sprite->y >> 3;
    uint8_t offsety = sprite->y & 0x7;
    ssd1306_setPos(sprite->x, posy);
    ssd1306_i2cDataStart();
    for (uint8_t i=0; i < sprite->w; i++)
    {
       ssd1306_i2cSendByte( B00000000 );
    }
    ssd1306_i2cStop();
    if (offsety)
    {
        ssd1306_setPos(sprite->x, posy + 1);
        ssd1306_i2cDataStart();
        for (uint8_t i=0; i < sprite->w; i++)
        {
           ssd1306_i2cSendByte( B00000000 );
        }
    }
    ssd1306_i2cStop();
}


void ssd1306_eraseTrace(SPRITE *sprite)
{
    uint8_t y1 = sprite->ly >> 3;
    uint8_t y2 = (sprite->ly + 7) >> 3;
    if (sprite->ly < sprite->y)
        y2 = min(y2, (uint8_t)((sprite->y >> 3) - 1));
    else
        y1 = max(y1, (sprite->ly + 7) >> 3);
    for(uint8_t y = y1; y <= y2; y++)
    {
        ssd1306_setPos(sprite->lx, y);
        ssd1306_i2cDataStart();
        for(uint8_t x = sprite->lx; x < sprite->lx + sprite->w; x++)
        {
            ssd1306_i2cSendByte( B00000000 );
        }
        ssd1306_i2cStop();
    }
    if (sprite->lx != sprite->x)
    {
        uint8_t x1 = sprite->lx;
        uint8_t x2 = sprite->lx + sprite->w - 1;
        if (sprite->x < sprite->lx)
            x1 = max(x1, sprite->x + sprite->w);
        else
            x2 = min((uint8_t)(sprite->x - 1), x2);
        for(uint8_t y = sprite->ly >> 3; y <= (sprite->ly + 7) >> 3; y++)
        {
            ssd1306_setPos(x1, y);
            ssd1306_i2cDataStart();
            for(uint8_t x = x1; x <= x2; x++)
            {
                ssd1306_i2cSendByte( B00000000 );
            }
            ssd1306_i2cStop();
        }
    }
}


SSD1306_RECT ssd1306_rectFromSprite(uint8_t x, uint8_t y, uint8_t w)
{
    return { x, (uint8_t)(y & 0xF8), (uint8_t)(x + w - 1), (uint8_t)((y + 7) & 0xF8) };
}

SSD1306_RECT ssd1306_rect(SPRITE * sprite)
{
    return { 0, 0, 0, 0 };
}


SPRITE       ssd1306_createSprite(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data)
{
    return {x,y,w,x,y,data};
}


