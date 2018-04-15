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

if len(sys.argv) < 2:
    print "Usage: glcd2ssd1306.py inputFile > outputFile"
    print "Examples:"
    print "      glcd2ssd1306.py glcdfont.c > ssd1306font.c"
    exit(1)

with open(sys.argv[1]) as f:
    content = f.readlines()

for line in content:
    bytes = line.split(', ')
    m = re.search(r'(const unsigned short)\s+(.+?)(\d+)x(\d+)', line)
    if m is not None:
        width = int(m.group(3))
        height = int(m.group(4))
        print "const uint8_t %s%dx%d[] PROGMEM =" % (m.group(2), width, height)
        print "{"
        print "    0x%02X, 0x%02X, 0x%02X, 0x%02X, " % (0, width, height, 32)
    elif len(bytes) >= 3:
    # convert line with data
        print "   ",
        heightInBytes = (height+7)/8
        for y in range(heightInBytes):
            for x in range(width):
                index = y + x * heightInBytes + 1
                if bytes[index].find("//") > 0:
                    print "%s" % bytes[index],
                else:
                    print "%s," % bytes[index],
        print "%s" % ''.join(bytes[heightInBytes*width + 1:]),
print "};"
