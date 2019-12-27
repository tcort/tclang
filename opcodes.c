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

#include <stdio.h>
#include <stdlib.h>

#include "call.h"
#include "opcodes.h"
#include "stack.h"
#include "symtab.h"
#include "types.h"

void op_add(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) + popstack(&vm->stack));
}

void op_and(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) & popstack(&vm->stack));
}

void op_bez(vm_t *vm) {
	if (popstack(&vm->stack) == 0) {
		vm->pc = symfind(&vm->symtab, vm->program.lines[vm->pc] + 12);
	}
}

void op_bnz(vm_t *vm) {
	if (popstack(&vm->stack) != 0) {
		vm->pc = symfind(&vm->symtab, vm->program.lines[vm->pc] + 12);
	}
}

void op_bra(vm_t *vm) {
	vm->pc = symfind(&vm->symtab, vm->program.lines[vm->pc] + 12);
}

void op_bls(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) << popstack(&vm->stack));
}

void op_brs(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) >> popstack(&vm->stack));
}

void op_ceq(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) == popstack(&vm->stack));
}

void op_cge(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) >= popstack(&vm->stack));
}

void op_cgt(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) > popstack(&vm->stack));
}

void op_cle(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) <= popstack(&vm->stack));
}

void op_clt(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) < popstack(&vm->stack));
}

void op_cne(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) != popstack(&vm->stack));
}

void op_dec(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) - 1);
}

void op_div(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) / popstack(&vm->stack));
}

void op_dup(vm_t *vm) {
	int32_t val = popstack(&vm->stack);
	pushstack(&vm->stack, val);
	pushstack(&vm->stack, val);
}

void op_hlt(vm_t *vm) {
	vm->done = 1;
}

void op_inc(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) + 1);
}

void op_jal(vm_t *vm) {
	call_link(&vm->call_stack, vm->pc);
	vm->pc = symfind(&vm->symtab, vm->program.lines[vm->pc] + 12);
}

void op_lda(vm_t *vm) {
	pushstack(&vm->stack, vm->memory[atoi(vm->program.lines[vm->pc] + 12)]);
}

void op_ldi(vm_t *vm) {
	pushstack(&vm->stack, atoi(vm->program.lines[vm->pc] + 12));
}

void op_mod(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) % popstack(&vm->stack));
}

void op_mul(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) * popstack(&vm->stack));
}

void op_not(vm_t *vm) {
	pushstack(&vm->stack, ~popstack(&vm->stack));
}

void op_oar(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) | popstack(&vm->stack));
}

void op_out(vm_t *vm) {
	fprintf(stdout, "%d\n", popstack(&vm->stack));
}

void op_prn(vm_t *vm) {
	fprintf(stdout, "%s\n", vm->program.lines[vm->pc] + 12);
}

void op_rtn(vm_t *vm) {
	vm->pc = call_return(&vm->call_stack);
}

void op_sta(vm_t *vm) {
	vm->memory[atoi(vm->program.lines[vm->pc] + 12)] = popstack(&vm->stack);
}

void op_sub(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) - popstack(&vm->stack));
}

void op_xor(vm_t *vm) {
	pushstack(&vm->stack, popstack(&vm->stack) ^ popstack(&vm->stack));
}
