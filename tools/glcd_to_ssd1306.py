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
# Fixed font converter for GLCD Creator fonts to internal ssd1306 format
#

import os
import sys
import re
import codecs

if len(sys.argv) < 2:
    print "Usage: glcd2ssd1306.py inputFile [options] > outputFile"
    print "Options:"
    print "    utf8      read source file as utf8-encoded"
    print "              utf8 option enforces script to generate unicode block for the font"
    print "              unicode block is part of ssd1306 font"
    print "Examples:"
    print "      glcd2ssd1306.py glcdfont.c > ssd1306font.c"
    print "      glcd2ssd1306.py glcdfont.c utf8 > ssd1306font.c"
    exit(1)

class GLCDFont:
    width = 0
    height = 0
    first_char = 32;
    name = "noname"
    chars = []
    charMap = {}

    def __init__(self, filename, enc = None):
        if enc is None:
            with open(filename) as f:
                content = f.readlines()
        else:
            with codecs.open(filename,'r',encoding=enc) as f:
                content = f.readlines()

        for line in content:
            bytes_str = line.split(', ')
            m = re.search(r'(const unsigned short)\s+(.+?)(\d+)x(\d+)', line)
            if m is not None:
                self.width = int(m.group(3))
                self.height = int(m.group(4))
                self.first_char = 32
                self.name = "%s%dx%d" % (m.group(2), self.width, self.height)
            elif len(bytes_str) >= 3:
            # convert line with data
                bytes = []
                for e in bytes_str:
                   if e.find("//") < 0:
                       bytes.append( int(e, 16) )
                   else:
                       if re.search(r'0x\d{2}', e) is not None:
                           bytes.append( int(e[:4], 16) )
                       m = re.search(r'char\s(.\w*)', e)
                       if m is not None:
                           char = m.group(1)
                           if char == "BackSlash":
                               char = "\\"
                           if len(self.chars) == 0:
                               self.first_char = ord(char[0])
                           self.chars.append( char )
                           self.charMap[ char ] = {}
                       else:
                           print "Parsing error"
                           exit(1)
                self.charMap[ char ]['width'] = self.width
                self.charMap[ char ]['used_width'] = bytes[0]
                self.charMap[ char ]['height'] = self.height
                self.charMap[ char ]['source_str'] = e
                self.charMap[ char ]['source_data'] = bytes
                self.charMap[ char ]['bitmap'] = []
                for y in range(self.height):
                   self.charMap[ char ]['bitmap'].append( [] )
                   for x in range(self.width):
                       index = y / 8 + x * (self.rows()) + 1
                       bit = y % 8
                       self.charMap[ char ]['bitmap'][y].append( (bytes[index] >> bit) & 0x01 )

    def printChar(self, char):
        print char
        print self.charMap[char]['source_data']
        for row in self.charMap[char]['bitmap']:
            print ''.join(str(x) for x in row)

    def charBitmap(self, char):
        return self.charMap[ char ]['bitmap']

    def rows(self):
        return (self.height + 7) / 8

enc = None
if len(sys.argv) > 2:
    enc = sys.argv[2]
font = GLCDFont(sys.argv[1], enc)
# print font.printChar( "B" )
# print font.charMap["B"]['source_str']

print "const uint8_t %s[] PROGMEM =" % (font.name)
print "{"
print "#ifdef CONFIG_SSD1306_UNICODE_ENABLE"
print "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (1, font.width, font.height, font.first_char)
print "    0x%02X, 0x%02X, 0x%02X, // unicode record" % \
       (font.first_char & 0xFF, (font.first_char >> 8) & 0xFF, \
        len(font.chars) & 0xFF)
print "#else"
print "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (0, font.width, font.height, font.first_char)
print "#endif"
char_code = font.first_char
for char in font.chars:
    print "   ",
    for row in range(font.rows()):
        for x in range(font.width):
            data = 0
            for i in range(8):
                y = row * 8 + i
                if y >= font.height:
                    break
                data |= (font.charBitmap(char)[y][x] << i)
            print "0x%02X," % data,
    print "// char '%s' (0x%04X/%d)" % (unichr(char_code), char_code, char_code)
    char_code = char_code + 1
print "#ifdef CONFIG_SSD1306_UNICODE_ENABLE"
print "    0x00, 0x00, 0x00, // end of unicode tables"
print "#endif"
print "};"
