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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "call.h"
#include "const.h"
#include "stack.h"
#include "symtab.h"
#include "types.h"
#include "util.h"

#include "op_add.h"
#include "op_bra.h"
#include "op_bez.h"
#include "op_bnz.h"
#include "op_ceq.h"
#include "op_cge.h"

static void cgt(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) > popstack(&vm->stack)); }
static void cle(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) <= popstack(&vm->stack)); }
static void clt(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) < popstack(&vm->stack)); }
static void cne(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) != popstack(&vm->stack)); }
static void xdiv(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) / popstack(&vm->stack)); }
static void dec(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) - 1); }
static void dup(vm_t *vm) { int32_t val = popstack(&vm->stack); pushstack(&vm->stack, val); pushstack(&vm->stack, val); }
static void hlt(vm_t *vm) { vm->done = 1; }
static void inc(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) + 1); }
static void jal(vm_t *vm) { call_link(&vm->call_stack, vm->pc); vm->pc = symfind(&vm->symtab, vm->program.lines[vm->pc] + 12); }
static void lda(vm_t *vm) { pushstack(&vm->stack, vm->memory[atoi(vm->program.lines[vm->pc] + 12)]); }
static void ldi(vm_t *vm) { pushstack(&vm->stack, atoi(vm->program.lines[vm->pc] + 12)); }
static void mod(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) % popstack(&vm->stack)); }
static void mul(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) * popstack(&vm->stack)); }
static void out(vm_t *vm) { fprintf(stdout, "%d\n", popstack(&vm->stack)); }
static void prn(vm_t *vm) { fprintf(stdout, "%s\n", vm->program.lines[vm->pc] + 12); }
static void rtn(vm_t *vm) { vm->pc = call_return(&vm->call_stack); }
static void sta(vm_t *vm) { vm->memory[atoi(vm->program.lines[vm->pc] + 12)] = popstack(&vm->stack); }
static void sub(vm_t *vm) { pushstack(&vm->stack, popstack(&vm->stack) - popstack(&vm->stack)); }

#define NOPS (25)
static op_t opcodes[NOPS] = {
	{ { 'A', 'D', 'D', '\0' }, { 0, 0, 0, 0 }, op_add },
	{ { 'B', 'R', 'A', '\0' }, { 0, 0, 0, 0 }, op_bra },
	{ { 'B', 'E', 'Z', '\0' }, { 0, 0, 0, 0 }, op_bez },
	{ { 'B', 'N', 'Z', '\0' }, { 0, 0, 0, 0 }, op_bnz },
	{ { 'C', 'E', 'Q', '\0' }, { 0, 0, 0, 0 }, op_ceq },
	{ { 'C', 'G', 'E', '\0' }, { 0, 0, 0, 0 }, op_cge },
	{ { 'C', 'G', 'T', '\0' }, { 0, 0, 0, 0 }, cgt },
	{ { 'C', 'L', 'E', '\0' }, { 0, 0, 0, 0 }, cle },
	{ { 'C', 'L', 'T', '\0' }, { 0, 0, 0, 0 }, clt },
	{ { 'C', 'N', 'E', '\0' }, { 0, 0, 0, 0 }, cne },
	{ { 'D', 'I', 'V', '\0' }, { 0, 0, 0, 0 }, xdiv },
	{ { 'D', 'E', 'C', '\0' }, { 0, 0, 0, 0 }, dec },
	{ { 'D', 'U', 'P', '\0' }, { 0, 0, 0, 0 }, dup },
	{ { 'H', 'L', 'T', '\0' }, { 0, 0, 0, 0 }, hlt },
	{ { 'I', 'N', 'C', '\0' }, { 0, 0, 0, 0 }, inc },
	{ { 'J', 'A', 'L', '\0' }, { 0, 0, 0, 0 }, jal },
	{ { 'L', 'D', 'A', '\0' }, { 0, 0, 0, 0 }, lda },
	{ { 'L', 'D', 'I', '\0' }, { 0, 0, 0, 0 }, ldi },
	{ { 'M', 'O', 'D', '\0' }, { 0, 0, 0, 0 }, mod },
	{ { 'M', 'U', 'L', '\0' }, { 0, 0, 0, 0 }, mul },
	{ { 'O', 'U', 'T', '\0' }, { 0, 0, 0, 0 }, out },
	{ { 'P', 'R', 'N', '\0' }, { 0, 0, 0, 0 }, prn },
	{ { 'R', 'T', 'N', '\0' }, { 0, 0, 0, 0 }, rtn },
	{ { 'S', 'T', 'A', '\0' }, { 0, 0, 0, 0 }, sta },
	{ { 'S', 'U', 'B', '\0' }, { 0, 0, 0, 0 }, sub }

};


void load(vm_t *vm) {

	char line[96]; /* TODO move to const.h */
	int cap = 96;

	memset(vm, '\0', sizeof(vm_t));

	while (fgets(line, cap, stdin) != NULL) {

		chomp(line);

		/* it's a label */
		if (line[0] != '#' && line[0] != ' ') {
			symdef(&vm->symtab, line, vm->program.sp);
		}
		/* record line for future reference */
		strncpy(vm->program.lines[vm->program.sp], line, LNLEN);
		vm->program.sp++;
	}
}

void run(vm_t *vm) {

	size_t begin;

	/* start a MAIN */
	begin = symfind(&vm->symtab, "MAIN"); /* move to const.h */
	/* if not found, start at line 0 */
	if (begin == LNMAX + 1) {
		begin = 0;
	}

	for (vm->pc = begin; vm->pc < vm->program.sp && !vm->done; vm->pc++) {
		size_t i;
		if (vm->program.lines[vm->pc][0] != ' ') {
			continue; /* label or comment */
		}
		/* opcode */
		for (i = 0; i < NOPS; i++) {
			/* find op code and execute it */
			if (memcmp(vm->program.lines[vm->pc] + 8, opcodes[i].code, 3) == 0) {
				opcodes[i].fn(vm);
				break;
			}
		}
		/* TODO probably want to abort here if no op code found */
	}

}

