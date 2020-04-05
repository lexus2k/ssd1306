# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2018-2020, Alexey Dynda
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

from __future__ import print_function
import sys

class Generator:
    source = None

    def __init__(self, source):
        self.source = source

    def generate_fixed_old(self):
        self.source.expand_chars()
        print("extern const uint8_t {0}[] PROGMEM;".format(self.source.name))
        print("const uint8_t {0}[] PROGMEM =".format(self.source.name))
        print("{")
        print("#ifdef CONFIG_SSD1306_UNICODE_ENABLE")
        print("//  type|width|height|first char")
        print("    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (1, self.source.width, self.source.height, self.source.first_char))
        print("//  unicode(2B)|count")
        print("    0x%02X, 0x%02X, 0x%02X, // unicode record" % \
             ((self.source.first_char >> 8) & 0xFF, self.source.first_char & 0xFF, \
              len(self.source.get_group_chars()) & 0xFF))
        print("#else")
        print("//  type|width|height|first char")
        print("    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (0, self.source.width, self.source.height, self.source.first_char))
        print("#endif")
        char_code = self.source.first_char
        for char in self.source.get_group_chars():
            print("    ", end='')
            for row in range(self.source.rows()):
                for x in range(self.source.width):
                    data = 0
                    for i in range(8):
                        y = row * 8 + i
                        if y >= self.source.height:
                            break
                        data |= (self.source.charBitmap(char)[y][x] << i)
                    print("0x%02X," % data, end=' ')
            if sys.version_info < (3, 0):
                print("// char '%s' (0x%04X/%d)" % (char.encode("utf-8"), char_code, char_code))
            else:
                print("// char '%s' (0x%04X/%d)" % (char, char_code, char_code))
            char_code = char_code + 1
        print("#ifdef CONFIG_SSD1306_UNICODE_ENABLE")
        print("    0x00, 0x00, 0x00, // end of unicode tables")
        print("#endif")
        print("};")

    def generate_new_format(self):
        total_size = 4
        self.source.expand_chars_top()
        print("extern const uint8_t %s[] PROGMEM;" % ("free_" + self.source.name))
        print("const uint8_t %s[] PROGMEM =" % ("free_" + self.source.name))
        print("{")
        print("//  type|width|height|first char")
        print("    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (2, self.source.width, self.source.height, 0x00))
        for group in range(self.source.groups_count()):
            chars = self.source.get_group_chars(group)
            total_size += 3
            if sys.version_info < (3, 0):
                print("// GROUP first '%s' total %d chars" % (chars[0].encode("utf-8"), len(chars)))
            else:
                chars  = list(chars)
                print("// GROUP first '%s' total %d chars" % (chars[0], len(chars)))
            print("//  unicode(LSB,MSB)|count")
            print("    0x%02X, 0x%02X, 0x%02X, // unicode record" % \
                 ((ord(chars[0]) >> 8) & 0xFF, ord(chars[0]) & 0xFF, len(chars) & 0xFF))
            # jump table
            offset = 0
            heights = []
            sizes = []
            for char in chars:
                bitmap = self.source.charBitmap(char)
                print("    ", end = '')
                width = len(bitmap[0])
                height = len(bitmap)
                while (height > 0) and (sum(bitmap[height -1]) == 0):
                    height -= 1
                heights.append( height )
                size = int((height + 7) / 8) * width
                sizes.append( size )
                total_size += 4
                print("0x%02X, 0x%02X, 0x%02X, 0x%02X," % (offset >> 8, offset & 0xFF, width, height), end = '')
                if sys.version_info < (3, 0):
                    print("// char '%s' (0x%04X/%d)" % (char.encode("utf-8"), ord(char), ord(char)))
                else:
                    print("// char '%s' (0x%04X/%d)" % (char, ord(char), ord(char)))
                offset += size
            total_size += 2
            print("    0x%02X, 0x%02X," % (offset >> 8, offset & 0xFF))
            # char data
            for index in range(len(chars)):
                char = chars[index]
                bitmap = self.source.charBitmap(char)
                print ("    ", end='')
                size = 0
                for row in range(int((heights[index] + 7) / 8)):
                    for x in range(len(bitmap[0])):
                        data = 0
                        for i in range(8):
                            y = row * 8 + i
                            if y >= len(bitmap):
                                break
                            data |= (self.source.charBitmap(char)[y][x] << i)
                        total_size += 1
                        size += 1
                        print("0x%02X," % data, end=' ')
                if sys.version_info < (3, 0):
                    print("// char '%s' (0x%04X/%d)" % (char.encode("utf-8"), ord(char), ord(char)))
                else:
                    print("// char '%s' (0x%04X/%d)" % (char, ord(char), ord(char)))
                if size != sizes[index]:
                    print("ERROR!!!!")
                    exit(1)
        total_size += 3
        print("    0x00, 0x00, 0x00, // end of unicode tables")
        print("    // FONT REQUIRES %d BYTES" % (total_size))
        print("};")

