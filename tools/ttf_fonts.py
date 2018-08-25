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
# pip install fonttools
#
# Get fonts
# wget https://ftp.gnu.org/gnu/freefont/freefont-ttf-20120503.zip

import sys
import freetype

if len(sys.argv) < 2:
    print "Usage: ttf_fonts.py ttfFile [size] > outputFile"
    print "Examples:"
    print "      ttf_fonts.py FreeSans.ttf 16 > ssd1306font.c"
    exit(1)

fsize = 8
if len(sys.argv) > 2:
    fsize = int(sys.argv[2])
fname = sys.argv[1]

face = freetype.Face(fname)
face.set_char_size( width=0, height=(fsize << 6), hres=96, vres=96 )
face.load_char(u'A', freetype.FT_LOAD_MONOCHROME | freetype.FT_LOAD_RENDER )
bitmap = face.glyph.bitmap

print bitmap.width, "x", bitmap.rows
print face.glyph.bitmap_left, ",", face.glyph.bitmap_top

for y in range(bitmap.rows):
   s=""
   for x in range(bitmap.width):
       index = y * bitmap.pitch + x / 8
       bit = 7 - (x % 8)
       s += "%01X" % ((bitmap.buffer[ index ] >> bit) & 1)
   print s



