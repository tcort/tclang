/******************************************************************************
Copyright (c) 2019 Thomas Cort

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "stack.h"
#include "types.h"

cell_t peekstack(vm_t *vm) {
	if (vm->stack.sp == 0) {
		return 0;
	}
	return vm->stack.mem[vm->stack.sp-1];
}

cell_t popstack(vm_t *vm) {
	if (vm->stack.sp == 0) {
		return 0;
	}
	vm->stack.sp--;
	return vm->stack.mem[vm->stack.sp];
}

void pushstack(vm_t *vm, cell_t c) {
	if (vm->stack.sp + 1 >= STKSZ) {
		return;
	}

	vm->stack.mem[vm->stack.sp] = c;
	vm->stack.sp++;
}

