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

import sys
import freetype

if len(sys.argv) < 2:
    print "Usage: ttf_fonts.py ttfFile [size] > outputFile"
    print "Examples:"
    print "      ttf_fonts.py FreeSans 16 > ssd1306font.c"
    print "Font name must not include .ttf extenstion. Tool will add it"
    exit(1)


class FontSource:
    width = 0
    height = 0
    # First char is for old format compatibility only
    first_char = None
    name = " "
    face = None
    # Char list is for compatibility with old format
    chars = []
    __groups = []

    def __init__(self, fontname, size):
        self.fontname = fontname
        self.name = self.fontname
        self.size = size
        self.face = freetype.Face( fontname + ".ttf" )
        self.face.set_char_size( width=0, height=(size << 6), hres=96, vres=96 )

    def add_chars(self, first, last):
        self.__groups.append([])
        if self.first_char is None:
            self.first_char = ord(first)
        index = len(self.__groups) - 1
        for code in range(ord(first), ord(last) + 1):
            ch = unichr( code )
            self.__add_char(index, ch)
        self.__commit_updates()

    def __add_char(self, group_index, ch):
        self.chars.append(ch)
        self.__groups[group_index].append( {} )
        index = len(self.__groups[group_index]) - 1
        self.face.load_char(ch, freetype.FT_LOAD_MONOCHROME | freetype.FT_LOAD_RENDER )
        bitmap = self.face.glyph.bitmap
        self.__groups[group_index][index]['char'] = ch
        self.__groups[group_index][index]['width'] = bitmap.width
        self.__groups[group_index][index]['used_width'] = bitmap.width
        self.__groups[group_index][index]['height'] = bitmap.rows
        self.__groups[group_index][index]['source_data'] = bitmap.buffer
        self.__groups[group_index][index]['left'] = self.face.glyph.bitmap_left
        self.__groups[group_index][index]['top'] = self.face.glyph.bitmap_top
        self.__groups[group_index][index]['bitmap'] = []
        for y in range( bitmap.rows ):
            self.__groups[group_index][index]['bitmap'].append( [] )
            for x in range( bitmap.width ):
                b_index = y * bitmap.pitch + x / 8
                bit = 7 - (x % 8)
                bit_data = (bitmap.buffer[ b_index ] >> bit) & 1
                self.__groups[group_index][index]['bitmap'][y].append( bit_data )

    def printChar(self, ch):
        data = self.__find_char_data(ch)
        if data is None:
            return
        print ch
        print data
        print data['source_data']
        for row in data['bitmap']:
            print "".join('-' if x == 0 else '@' for x in row)

    def printString(self, s):
        for y in range(self.height):
            row = ""
            for ch in s:
               data = self.__find_char_data(ch)
               row += "".join('-' if x == 0 else '@' for x in data['bitmap'][y])
               row += "-"
            print row

    def charBitmap(self, ch):
        data = self.__find_char_data(ch)
        if data is None:
            return None
        return data['bitmap']

    def rows(self):
        return (self.height + 7) / 8

    def __find_char_data(self, ch):
        res = []
        for g in self.__groups:
            res = filter(lambda x: x['char'] == ch, g)
            if len(res) > 0:
                break
        return None if len(res) == 0 else res[0]

    # Function calculates
    #   * baseline for the font (the line, where all chars are attached to)
    #   * width for the font (the width of most wide character)
    #   * height for the font (the height of most tall character)
    def __commit_updates(self):
        top = 0
        bottom = 0
        left = 0
        right = 0
        for g in self.__groups:
            for c in g:
                top = min([top, 0 - c['top']])
                bottom = max([bottom, c['height'] - c['top']])
                left = min([left, 0 - c['left']])
                right = max([right, c['width'] - c['left']])
        self.width = right - left
        self.height = bottom - top
        self.baseline = -top
        self.baseline_h = 0

    # Function expands character bitmap vertically to match the tallest char
    def __expand_char_v(self, data):
        # expand top
        for y in range(0, self.baseline - data['top']):
            data['bitmap'].insert(0, [0] * data['width'])
        for y in range(0, self.height - self.baseline - (data['height'] - data['top'])):
            data['bitmap'].append( [0] * data['width'])
        data['top'] = self.baseline
        data['height'] = self.height

    # Function expands character bitmap horizontally to match the widest char
    def __expand_char_h(self, data):
        # expand top
        if self.baseline_h == 0:
            before = (self.width - data['width']) / 2
            after = self.width - data['width'] - before
        else:
            before = self.baseline_h - data['left']
            after = self.width - self.baseline_h - (data['width'] - data['left'])
        if before > 0:
            for d in data['bitmap']:
               for n in range(before):
                   d.insert(0, 0)
        if after > 0:
            for d in data['bitmap']: d += [0]*after
        data['left'] = self.baseline_h
        data['width'] = self.width

    # Function expands all chars vertically to match the tallest char
    def expand_chars_v(self):
        for g in self.__groups:
            for c in g:
                self.__expand_char_v( c )

    # Function expans all chars horizontally to match the widest char
    def expand_chars_h(self):
        for g in self.__groups:
            for c in g:
                self.__expand_char_h( c )

    # Function expands all char and makes them fixed width and fixed height
    def expand_chars(self):
        self.expand_chars_v()
        self.expand_chars_h()


class FontGenerator:
    source = None

    def __init__(self, source):
        self.source = source

    def generate_fixed_old(self):
        self.source.expand_chars()
        print "const uint8_t %s[] PROGMEM =" % (self.source.name)
        print "{"
        print "#ifdef CONFIG_SSD1306_UNICODE_ENABLE"
        print "//  type|width|height|first char"
        print "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (1, self.source.width, self.source.height, self.source.first_char)
        print "//  unicode(2B)|count"
        print "    0x%02X, 0x%02X, 0x%02X, // unicode record" % \
             (self.source.first_char & 0xFF, (self.source.first_char >> 8) & 0xFF, \
              len(self.source.chars) & 0xFF)
        print "#else"
        print "//  type|width|height|first char"
        print "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (0, self.source.width, self.source.height, self.source.first_char)
        print "#endif"
        char_code = self.source.first_char
        for char in self.source.chars:
            print "   ",
            for row in range(self.source.rows()):
                for x in range(self.source.width):
                    data = 0
                    for i in range(8):
                        y = row * 8 + i
                        if y >= self.source.height:
                            break
                        data |= (self.source.charBitmap(char)[y][x] << i)
                    print "0x%02X," % data,
            print "// char '%s' (0x%04X/%d)" % (char, char_code, char_code)
            char_code = char_code + 1
        print "#ifdef CONFIG_SSD1306_UNICODE_ENABLE"
        print "    0x00, 0x00, 0x00, // end of unicode tables"
        print "#endif"
        print "};"

fsize = 8
if len(sys.argv) > 2:
    fsize = int(sys.argv[2])
fname = sys.argv[1]

source = FontSource(fname, fsize)
source.add_chars(' ', unichr(127))

#source.printChar('q')
#source.expand_chars_v()
#source.printString('World!q01')
#source.printChar('q')
#source.printChar('Q')

font = FontGenerator( source )

font.generate_fixed_old()
