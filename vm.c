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

#include "config.h"

#include <limits.h>
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

#define op(NAME, FUNC) { { NAME }, { 0, 0, 0, 0 }, FUNC }

#define NOPS (34)
static op_t opcodes[NOPS] = {
	op("ADD", op_add),
	op("AND", op_and),
	op("BEZ", op_bez),
	op("BLS", op_bls),
	op("BNZ", op_bnz),
	op("BRA", op_bra),
	op("BRS", op_brs),
	op("CEQ", op_ceq),
	op("CGE", op_cge),
	op("CGT", op_cgt),
	op("CLE", op_cle),
	op("CLT", op_clt),
	op("CNE", op_cne),
	op("DEC", op_dec),
	op("DIV", op_div),
	op("DUP", op_dup),
	op("HLT", op_hlt),
	op("ICH", op_ich),
	op("INC", op_inc),
	op("INI", op_ini),
	op("JAL", op_jal),
	op("LDA", op_lda),
	op("LDI", op_ldi),
	op("MOD", op_mod),
	op("MUL", op_mul),
	op("NOT", op_not),
	op("OAR", op_oar),
	op("OCH", op_och),
	op("OTI", op_oti),
	op("OTS", op_ots),
	op("RTN", op_rtn),
	op("STA", op_sta),
	op("SUB", op_sub),
	op("XOR", op_xor)
};

void load(vm_t *vm, FILE *in) {

	char line[LINE_MAX];
	int cap = LINE_MAX;

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

	for (vm->pc = begin; vm->pc < vm->program.sp && !vm->done && !feof(stdin) && !ferror(stdin); vm->pc++) {
		size_t i, run;
		if (vm->program.lines[vm->pc][0] != ' ') {
			continue; /* label or comment */
		}
		/* opcode */
		for (run = i = 0; i < NOPS; i++) {
			/* find op code and execute it */
			if (memcmp(vm->program.lines[vm->pc] + 8, opcodes[i].code, 3) == 0) {
				opcodes[i].fn(vm);
				run = 1;
				break;
			}
		}
		if (run == 0) {
			fprintf(stderr, "ERROR: BAD OP CODE (LINE %lu)\n", vm->pc);
			return;
		}
	}

}

