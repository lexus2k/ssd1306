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
#

import os
import sys
import re
import codecs
from . import fontcontainer

class GLCDSource(fontcontainer.FontContainer):

    def __init__(self, filename, size, enc):
        fontcontainer.FontContainer.__init__(self, filename, size)
        if enc is None:
            with open(filename) as f:
                content = f.readlines()
        else:
            with codecs.open(filename,'r',encoding=enc) as f:
                content = f.readlines()

        self.add_group()
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
                           print("Parsing error")
                           exit(1)
                # Cast bitmap from parsed data
                bitmap=[]
                for y in range(height):
                   bitmap.append( [] )
                   for x in range(width):
                       row = y / 8 + x * ((height + 7) / 8) + 1
                       bit = y % 8
                       bitmap[y].append( (bytes[row] >> bit) & 0x01 )
                # Add new char
                self.add_char(0, char, source=bytes,\
                                       bitmap=bitmap,\
                                       width=width,\
                                       height=height)
        self._commit_updates()
