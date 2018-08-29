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
# Fixed font converter for GLCD Creator fonts to internal ssd1306 format
#

import os
import sys
import re
import codecs
import fontcontainer

class GLCDSource(fontcontainer.FontContainer):

    def __init__(self, filename, size, enc):
        fontcontainer.FontContainer.__init__(self, filename, size)
        if enc is None:
            with open(filename) as f:
                content = f.readlines()
        else:
            with codecs.open(filename,'r',encoding=enc) as f:
                content = f.readlines()

        self._groups.append([])
        for line in content:
            bytes_str = line.split(', ')
            m = re.search(r'(const unsigned short)\s+(.+?)(\d+)x(\d+)', line)
            if m is not None:
                width = int(m.group(3))
                height = int(m.group(4))
                self.first_char = 32
                self._origin_name = "%s" % (m.group(2))
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
                           if len(self.get_group_chars()) == 0:
                               self.first_char = ord(char[0])
                       else:
                           print "Parsing error"
                           exit(1)
                self._groups[0].append({})
                index = len(self._groups[0]) - 1
                self._groups[0][index]['char'] = char
                self._groups[0][index]['width'] = width
                self._groups[0][index]['used_width'] = bytes[0]
                self._groups[0][index]['height'] = height
                self._groups[0][index]['source_str'] = e
                self._groups[0][index]['source_data'] = bytes
                self._groups[0][index]['bitmap'] = []
                self._groups[0][index]['top'] = height
                self._groups[0][index]['left'] = 0
                for y in range(height):
                   self._groups[0][index]['bitmap'].append( [] )
                   for x in range(width):
                       row = y / 8 + x * (self.rows()) + 1
                       bit = y % 8
                       self._groups[0][index]['bitmap'][y].append( (bytes[row] >> bit) & 0x01 )
        self._commit_updates()
