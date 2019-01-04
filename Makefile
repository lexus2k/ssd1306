#    MIT License
#
#    Copyright (c) 2019, Alexey Dynda
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
#################################################################

.PHONY: docs library help

help:
	@echo "============== SSD1306 library ================"
	@echo "make docs          generates documentation"
	@echo "make help          prints this help"
	@echo "make check         run cppcheck tests"
	@echo ""
	@echo "to build examples use scripts in tools subdir"

docs:
	@cd src && doxygen doxygen.cfg 1>/dev/null
	@echo "[DONE] check docs folder"

check:
	@cppcheck --force \
	    --enable=warning,style,performance,portability \
	    --suppress=information \
	    -q -I ./src \
	    --error-exitcode=1 .
	@echo "[DONE]"

