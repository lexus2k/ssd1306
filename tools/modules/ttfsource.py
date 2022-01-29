# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2018, Alexey Dynda
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.
#
###################################################################################
# Fixing pip
# curl https://bootstrap.pypa.io/get-pip.py | python2
# sudo pip install freetype-py
#
# Get fonts
# wget https://ftp.gnu.org/gnu/freefont/freefont-ttf-20120503.zip

from . import fontcontainer
import os
import sys
import freetype

class TTFSource(fontcontainer.FontContainer):
    face = None

    # filename - name of TTF font file (full path is supported)
    # size - size of font (not pixels)
    def __init__(self, filename, size):
        fontname = os.path.basename(os.path.splitext(filename)[0])
        fontcontainer.FontContainer.__init__(self, fontname, size)
        self.face = freetype.Face( filename )
        self.face.set_char_size( width=0, height=(size << 6), hres=96, vres=96 )

    def add_chars(self, first, last):
        index = self.add_group()
        if self.first_char is None:
            self.first_char = ord(first)
        for code in range(ord(first), ord(last) + 1):
            if sys.version_info < (3, 0):
                ch = unichr( code )
            else:
                ch = chr( code )
            self.__add_char(index, ch)
        self._commit_updates()

    def __add_char(self, group_index, ch):
        self.face.load_char(ch, freetype.FT_LOAD_MONOCHROME | freetype.FT_LOAD_RENDER )
        bitmap = self.face.glyph.bitmap
        # print(bitmap.buffer)
        bitmap_data = []
        for y in range( bitmap.rows ):
            bitmap_data.append( [] )
            for x in range( bitmap.width ):
                b_index = y * bitmap.pitch + int(x / 8)
                bit = 7 - int(x % 8)
                bit_data = (bitmap.buffer[ b_index ] >> bit) & 1
                bitmap_data[y].append( bit_data )
        self.add_char(group_index, ch,\
                      source=bitmap.buffer,\
                      bitmap=bitmap_data,\
                      width=bitmap.width,\
                      height=bitmap.rows,\
                      left=self.face.glyph.bitmap_left,\
                      top=self.face.glyph.bitmap_top)

