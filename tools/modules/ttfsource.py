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

import freetype

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

    def get_group_chars(self, group_index = -1):
        if group_index < 0:
            return self.chars
        return map(lambda x: x['char'], self.__groups[group_index])

    def groups_count(self):
        return len(self.__groups)

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
        # print ch
        # print data
        # print data['source_data']
        for row in data['bitmap']:
            print "".join('-' if x == 0 else '@' for x in row)

    def printString(self, s):
        for y in range(self.height):
            row = ""
            for ch in s:
               data = self.__find_char_data(ch)
               if data is None:
                   continue
               index = y - self.baseline + data['top']
               if index < 0 or index >= data['height']:
                   row += "".join(['-'] * (data['width'] + 1))
                   continue
               row += "".join('-' if x == 0 else '@' for x in data['bitmap'][index])
               row += "-"
            print "//", row

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
        self.baseline_h = -left

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

    # Function deflates character bitmap horizontally by left pixels
    # from left side and right pixels from right side
    def __deflate_char_h(self, data, left=0, right=0):
        for d in data['bitmap']:
            d = d[left:len(d)-right]
        data['width'] -= (left + right)
        data['left'] -= left
        if data['left'] < 0:
            data['left'] = 0

    def __deflate_char_v(self, data, top=0, bottom=0):
        data['bitmap'] = data['bitmap'][top:len(data['bitmap'])-bottom]
        data['height'] -= (top + bottom)
        data['top'] -= top
        if data['top'] < 0:
            data['top'] = 0

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

    def deflate_chars(self, left=0, top=0, right=0, bottom=0):
        # Calculate maximum parts according to requested change
        left_part = self.baseline_h - left
        right_part = self.width - self.baseline_h - right
        top_part = self.baseline - top
        bottom_part = self.height - self.baseline - bottom
        for g in self.__groups:
            for c in g:
                # Deflate char only if it is out of font size
                left_p = max([c['left'] - left_part, 0])
                right_p = max([c['width'] - c['left'] - right_part, 0])
                self.__deflate_char_h( c, left_p, right_p )
                top_p = max([c['top'] - top_part, 0])
                bottom_p = max([c['height'] - c['top'] - bottom_part, 0])
                self.__deflate_char_v( c, top_p, bottom_p )
        self.__commit_updates()

    # Deflate chars from bottom side to specified height
    def deflate_chars_bottom(self, height):
        bottom = self.height - height
        self.deflate_chars(bottom = bottom)

