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
from modules import glcdsource
from modules import fontgenerator

def print_help_and_exit():
    print "Usage: ttf_fonts.py [args] > outputFile"
    print "args:"
    print "      --ttf S   use ttf name as source"
    print "      --glcd S  use glcd file as as source"
    print "      -s <N>    font size (this is not pixels!) "
    print "      -SB <N>   limit size in pixels to value (pixels will be cut)"
    print "      -fh       fixed height"
    print "      -fw       fixed width"
    print "      -f old    old format 1.7.6 and below"
    print "      -f new    new format 1.7.8 and above"
    print "      -d        Print demo text to console"
    print "      --demo-only Prints demo text to console and exits"
    print "Examples:"
    print "   [convert ttf font to ssd1306 old format]"
    print "      ttf_fonts.py --ttf FreeSans.ttf -s 8 -f old > ssd1306font.h"
    print "   [convert ttf font to ssd1306 new format with demo text and print to console]"
    print "      ttf_fonts.py --ttf FreeSans.ttf -d -f new"
    print "   [convert GLCD font generated file to new format]"
    print "      ttf_fonts.py --glcd font.c -f new > ssd1306font.h"
    exit(1)

if len(sys.argv) < 2:
    print_help_and_exit()

fsize = 8
fold = False
flimit_bottom = 0
fwidth = False
fheight = False
fname = ""
TTF = False
GLCD = False
demo_text = False
generate_font = True
source = None

# parse args
idx = 1
while idx < len(sys.argv):
    opt = sys.argv[idx]
    if opt == "--ttf":
        TTF = True
        idx += 1
        fname = sys.argv[idx]
    elif opt == "--glcd":
        GLCD = True
        idx += 1
        fname = sys.argv[idx]
    elif opt == "-s":
        idx += 1
        fsize = int(sys.argv[idx])
    elif opt == "-SB":
        idx +=1
        flimit_bottom = int(sys.argv[idx])
    elif opt == "-fh":
        fheight = True
    elif opt == "-fw":
        fwidth = True
    elif opt == "-f":
        idx += 1
        if sys.argv[idx] == "old":
            fold = True
    elif opt == "-d":
        demo_text = True
    elif opt == "--demo-only":
        generate_font = False
        demo_text = True
    else:
        print "Unknown option: ", opt
        print_help_and_exit()
    idx += 1

if TTF:
    from modules import ttfsource
    source = ttfsource.TTFSource(fname, fsize)
    source.add_chars(' ', unichr(127))
if GLCD:
    source = glcdsource.GLCDSource(fname, fsize, "utf8")

if source is None:
    print_help_and_exit()

if fwidth:
    source.expand_chars_h()
if fheight:
    source.expand_chars_v()
if flimit_bottom > 0:
    source.deflate_chars_bottom( flimit_bottom )

font = fontgenerator.Generator( source )
if fold:
    source.expand_chars()
    if demo_text:
        source.printString('World!q01')
    if generate_font:
        font.generate_fixed_old()
else:
    if demo_text:
        source.printString('World!q01')
    if generate_font:
        font.generate_new_format()

