# Copyright (c) 2019 Thomas Cort
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

bin_PROGRAMS = tclang
tclang_SOURCES = \
	call.c    call.h \
	          const.h \
	main.c \
	op_add.c  op_add.h \
	op_bra.c  op_bra.h \
	op_bez.c  op_bez.h \
	op_bnz.c  op_bnz.h \
	op_ceq.c  op_ceq.h \
	op_cge.c  op_cge.h \
	op_cgt.c  op_cgt.h \
	op_cle.c  op_cle.h \
	op_clt.c  op_clt.h \
	stack.c   stack.h \
	symtab.c  symtab.h \
	          types.h \
	util.c    util.h \
	vm.c      vm.h

EXTRA_DIST = autogen.sh LICENSE.md README.md TODO.md
