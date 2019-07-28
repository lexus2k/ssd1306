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


# if len(sys.argv) < 2:
#    print "Usage: glcd2ssd1306.py inputFile > outputFile"
#    print "Examples:"
#    print "      glcd2ssd1306.py glcdfont.c > ssd1306font.c"
#    exit(1)

# filename = sys.argv[1]
filename = "../examples/games/lode_runner/sprites.cpp"

with open(filename) as f:
    content = f.read()

index = 0
#pattern = re.compile(r'(0x[0-9a-fA-F]{1,})')
#name_pattern = re.compile(r'(.*)')
name_pattern = re.compile(r'([\w\d_]+)\s*(PROGMEM\s*){0,1}(\[\s*[\w\d_]*?\s*\]|)*\s*=\s*\{(.*?)\};', re.DOTALL)
#name_pattern = re.compile(r'([\w\d_]+)\s*(PROGMEM\s*){0,1}(\[\s*[\w\d_]*?\s*\]|)*\s*=\s*\{([.\/\w\s\,{}]*?)\};')
pattern = re.compile(r'\{(\s*(0x[0-9a-fA-F]+|0B\d+)\s*,{0,1}\s*)*?\}')
#pattern = re.compile(r'\{(\s*0B\d+\s*,{0,1}\s*)*\}')

all = name_pattern.findall(content)
print all
exit(1)



n = name_pattern.search(content, re.MULTILINE)
print n.start(), n.end()
print n.group(1), n.group(4)

m = pattern.search(content, n.start(), n.end())
print m
print m.group(0)


