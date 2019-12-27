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
#include "opcodes.h"
#include "stack.h"
#include "symtab.h"
#include "types.h"
#include "util.h"

#define NOPS (31)
static op_t opcodes[NOPS] = {
	{ { 'A', 'D', 'D', '\0' }, { 0, 0, 0, 0 }, op_add },
	{ { 'A', 'N', 'D', '\0' }, { 0, 0, 0, 0 }, op_and },
	{ { 'B', 'L', 'S', '\0' }, { 0, 0, 0, 0 }, op_bls },
	{ { 'B', 'R', 'A', '\0' }, { 0, 0, 0, 0 }, op_bra },
	{ { 'B', 'R', 'S', '\0' }, { 0, 0, 0, 0 }, op_brs },
	{ { 'B', 'E', 'Z', '\0' }, { 0, 0, 0, 0 }, op_bez },
	{ { 'B', 'N', 'Z', '\0' }, { 0, 0, 0, 0 }, op_bnz },
	{ { 'C', 'E', 'Q', '\0' }, { 0, 0, 0, 0 }, op_ceq },
	{ { 'C', 'G', 'E', '\0' }, { 0, 0, 0, 0 }, op_cge },
	{ { 'C', 'G', 'T', '\0' }, { 0, 0, 0, 0 }, op_cgt },
	{ { 'C', 'L', 'E', '\0' }, { 0, 0, 0, 0 }, op_cle },
	{ { 'C', 'L', 'T', '\0' }, { 0, 0, 0, 0 }, op_clt },
	{ { 'C', 'N', 'E', '\0' }, { 0, 0, 0, 0 }, op_cne },
	{ { 'D', 'I', 'V', '\0' }, { 0, 0, 0, 0 }, op_div },
	{ { 'D', 'E', 'C', '\0' }, { 0, 0, 0, 0 }, op_dec },
	{ { 'D', 'U', 'P', '\0' }, { 0, 0, 0, 0 }, op_dup },
	{ { 'H', 'L', 'T', '\0' }, { 0, 0, 0, 0 }, op_hlt },
	{ { 'I', 'N', 'C', '\0' }, { 0, 0, 0, 0 }, op_inc },
	{ { 'J', 'A', 'L', '\0' }, { 0, 0, 0, 0 }, op_jal },
	{ { 'L', 'D', 'A', '\0' }, { 0, 0, 0, 0 }, op_lda },
	{ { 'L', 'D', 'I', '\0' }, { 0, 0, 0, 0 }, op_ldi },
	{ { 'M', 'O', 'D', '\0' }, { 0, 0, 0, 0 }, op_mod },
	{ { 'M', 'U', 'L', '\0' }, { 0, 0, 0, 0 }, op_mul },
	{ { 'N', 'O', 'T', '\0' }, { 0, 0, 0, 0 }, op_not },
	{ { 'O', 'A', 'R', '\0' }, { 0, 0, 0, 0 }, op_oar },
	{ { 'O', 'U', 'T', '\0' }, { 0, 0, 0, 0 }, op_out },
	{ { 'P', 'R', 'N', '\0' }, { 0, 0, 0, 0 }, op_prn },
	{ { 'R', 'T', 'N', '\0' }, { 0, 0, 0, 0 }, op_rtn },
	{ { 'S', 'T', 'A', '\0' }, { 0, 0, 0, 0 }, op_sta },
	{ { 'S', 'U', 'B', '\0' }, { 0, 0, 0, 0 }, op_sub },
	{ { 'X', 'O', 'R', '\0' }, { 0, 0, 0, 0 }, op_xor }
};


void load(vm_t *vm, FILE *in) {

	char line[96]; /* TODO move to const.h */
	int cap = 96;

	memset(vm, '\0', sizeof(vm_t));

	while (fgets(line, cap, in) != NULL) {

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

