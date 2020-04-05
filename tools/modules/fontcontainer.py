# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2018-2019, Alexey Dynda
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

import sys

class FontContainer:
    width = 0
    height = 0
    # First char is for old format compatibility only
    first_char = None
    name = " "
    face = None
    # Char list is for compatibility with old format
    _groups = []

    def __init__(self, name, size):
        self.width = 0
        self.height = 0
        self.size = size
        self._origin_name = name
        self.name = name

    def get_group_chars(self, group_index = -1):
        if group_index < 0:
            chars = []
            for i in range(len(self._groups)):
                chars.extend( self.get_group_chars(i) )
            return chars
        return map(lambda x: x['char'], self._groups[group_index])

    # Adds new group and returns its index
    def add_group(self):
        self._groups.append([])
        return len(self._groups) - 1

    def add_char(self, group, char, source, bitmap, width=-1, height=-1, left=-1, top=-1):
        self._groups[group].append({})
        index = len(self._groups[group]) - 1
        if width < 0:
            width = len(bitmap[0])
        if height < 0:
            height = len(bitmap)
        if left < 0:
            left = 0
        if top < 0:
            top = height
        self._groups[group][index]['char'] = char
        self._groups[group][index]['width'] = width
        self._groups[group][index]['used_width'] = width
        self._groups[group][index]['height'] = height
        self._groups[group][index]['source_data'] = source
        self._groups[group][index]['left'] = left
        self._groups[group][index]['top'] = top
        self._groups[group][index]['bitmap'] = bitmap

    def groups_count(self):
        return len(self._groups)

    def printChar(self, ch):
        data = self._find_char_data(ch)
        if data is None:
            return
        # print(ch)
        # print(data)
        # print(data['source_data'])
        for row in data['bitmap']:
            print("".join('-' if x == 0 else '@' for x in row))

    def printString(self, s):
        for y in range(self.height):
            row = ""
            for ch in s:
               data = self._find_char_data(ch)
               if data is None:
                   continue
               index = y - self.baseline + data['top']
               if index < 0 or index >= data['height']:
                   row += "".join(['-'] * (data['width'] + 1))
                   continue
               row += "".join('-' if x == 0 else '@' for x in data['bitmap'][index])
               row += "-"
            print("// {0}".format(row))

    def charBitmap(self, ch):
        data = self._find_char_data(ch)
        if data is None:
            return None
        return data['bitmap']

    def rows(self):
        return int((self.height + 7) / 8)

    def _find_char_data(self, ch):
        res = []
        for g in self._groups:
            res = filter(lambda x: x['char'] == ch, g)
            if sys.version_info >= (3, 0):
                res = list(res)
            if len(res) > 0:
                break
        return None if len(res) == 0 else res[0]

    # Function calculates
    #   * baseline for the font (the line, where all chars are attached to)
    #   * width for the font (the width of most wide character)
    #   * height for the font (the height of most tall character)
    def _commit_updates(self):
        top = 0
        bottom = 0
        left = 0
        right = 0
        for g in self._groups:
            for c in g:
                top = min([top, 0 - c['top']])
                bottom = max([bottom, c['height'] - c['top']])
                left = min([left, 0 - c['left']])
                right = max([right, c['width'] - c['left']])
        self.width = right - left
        self.height = bottom - top
        self.baseline = -top
        self.baseline_h = -left
        self.name = self._origin_name + str(self.width) + "x" + str(self.height)

    # Function expands character bitmap vertically to top boundary
    def __expand_char_top(self, data):
        # expand top
        for y in range(0, self.baseline - data['top']):
            data['bitmap'].insert(0, [0] * data['width'])
        data['top'] = self.baseline
        data['height'] = len(data['bitmap'])

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
            before = int((self.width - data['width']) / 2)
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

    # Function expans all chars horizontally to match the widest char
    def expand_chars_h(self):
        for g in self._groups:
            for c in g:
                self.__expand_char_h( c )

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
        if top < 0:
            data['bitmap'] = [ ([0] * data['width']) for x in range(-top) ] + data['bitmap']
            data['height'] -= top
            data['top'] -= top
            top = 0
        if bottom < 0:
            data['bitmap'].extend( [ ([0] * data['width']) for x in range(-bottom) ] )
            data['height'] -= bottom
            bottom = 0
        data['bitmap'] = data['bitmap'][top:len(data['bitmap'])-bottom]
        data['height'] -= (top + bottom)
        data['top'] -= top
        if data['top'] < 0:
            data['top'] = 0

    # Function expands all chars vertically to match the tallest char
    def expand_chars_top(self):
        for g in self._groups:
            for c in g:
                self.__expand_char_top( c )

    # Function expands all chars vertically to match the tallest char
    def expand_chars_v(self):
        for g in self._groups:
            for c in g:
                self.__expand_char_v( c )

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
        for g in self._groups:
            for c in g:
                # Deflate char only if it is out of font size
                left_p = max([c['left'] - left_part, 0])
                right_p = max([c['width'] - c['left'] - right_part, 0])
                self.__deflate_char_h( c, left_p, right_p )
                top_p = max([c['top'] - top_part, 0])
                # bottom_p = max([c['height'] - c['top'] - bottom_part, 0])
                bottom_p = c['height'] - c['top'] - bottom_part
                self.__deflate_char_v( c, top_p, bottom_p )
        self._commit_updates()

    # Deflate chars from bottom side to specified height
    def deflate_chars_bottom(self, height):
        bottom = self.height - height
        self.deflate_chars(bottom = bottom)

