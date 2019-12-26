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
#include "types.h"

static vm_t vm;


static size_t symfind(char *label);

static void add(void) { pushstack(&vm, popstack(&vm) + popstack(&vm)); }
static void bra(void) { vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void bez(void) { if (popstack(&vm) == 0) vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void bnz(void) { if (popstack(&vm) != 0) vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void ceq(void) { pushstack(&vm, popstack(&vm) == popstack(&vm)); }
static void cge(void) { pushstack(&vm, popstack(&vm) >= popstack(&vm)); }
static void cgt(void) { pushstack(&vm, popstack(&vm) > popstack(&vm)); }
static void cle(void) { pushstack(&vm, popstack(&vm) <= popstack(&vm)); }
static void clt(void) { pushstack(&vm, popstack(&vm) < popstack(&vm)); }
static void cne(void) { pushstack(&vm, popstack(&vm) != popstack(&vm)); }
static void xdiv(void) { pushstack(&vm, popstack(&vm) / popstack(&vm)); }
static void dec(void) { pushstack(&vm, popstack(&vm) - 1); }
static void dup(void) { int32_t val = popstack(&vm); pushstack(&vm, val); pushstack(&vm, val); }
static void hlt(void) { vm.done = 1; }
static void inc(void) { pushstack(&vm, popstack(&vm) + 1); }
static void jal(void) { call_link(&vm, vm.pc); vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void lda(void) { pushstack(&vm, vm.memory[atoi(vm.program.lines[vm.pc] + 12)]); }
static void ldi(void) { pushstack(&vm, atoi(vm.program.lines[vm.pc] + 12)); }
static void mod(void) { pushstack(&vm, popstack(&vm) % popstack(&vm)); }
static void mul(void) { pushstack(&vm, popstack(&vm) * popstack(&vm)); }
static void out(void) { fprintf(stdout, "%d\n", popstack(&vm)); }
static void prn(void) { fprintf(stdout, "%s\n", vm.program.lines[vm.pc] + 12); }
static void rtn(void) { vm.pc = call_return(&vm); }
static void sta(void) { vm.memory[atoi(vm.program.lines[vm.pc] + 12)] = popstack(&vm); }
static void sub(void) { pushstack(&vm, popstack(&vm) - popstack(&vm)); }

#define NOPS (25)
static op_t opcodes[NOPS] = {
	{ { 'A', 'D', 'D', '\0' }, { 0, 0, 0, 0 }, add },
	{ { 'B', 'R', 'A', '\0' }, { 0, 0, 0, 0 }, bra },
	{ { 'B', 'E', 'Z', '\0' }, { 0, 0, 0, 0 }, bez },
	{ { 'B', 'N', 'Z', '\0' }, { 0, 0, 0, 0 }, bnz },
	{ { 'C', 'E', 'Q', '\0' }, { 0, 0, 0, 0 }, ceq },
	{ { 'C', 'G', 'E', '\0' }, { 0, 0, 0, 0 }, cge },
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

static void chomp(char *line) {
	size_t end = strlen(line) - 1;
	if (line[end] == '\n') {
		line[end] = '\0';
		end--;
	}
	if (line[end] == '\r') {
		line[end] = '\0';
	}
}

static void load(void) {

	char line[96];
	int cap = 96;

	memset(&vm, '\0', sizeof(vm_t));

	while (fgets(line, cap, stdin) != NULL) {

		chomp(line);

		/* it's a label */
		if (line[0] != '#' && line[0] != ' ') {
			strncpy(vm.symtab.symbols[vm.symtab.sp].label, line, LBLLN);
			vm.symtab.symbols[vm.symtab.sp].lineno = vm.program.sp;
			vm.symtab.sp++;
		}
		/* record line for future reference */
		strncpy(vm.program.lines[vm.program.sp], line, LNLEN);
		vm.program.sp++;
	}
}

static size_t symfind(char *label) {
	size_t i;

	for (i = 0; i < vm.symtab.sp; i++) {	
		if (strcmp(vm.symtab.symbols[i].label, label) == 0) {
			return vm.symtab.symbols[i].lineno;
		}

	}

	return LNMAX + 1;
}

static void run(void) {

	size_t begin;

	/* start a MAIN */
	begin = symfind("MAIN");
	/* if not found, start at line 0 */
	if (begin == LNMAX + 1) {
		begin = 0;
	}

	for (vm.pc = begin; vm.pc < vm.program.sp && !vm.done; vm.pc++) {
		size_t i;
		if (vm.program.lines[vm.pc][0] != ' ') {
			continue; /* label or comment */
		}
		/* opcode */
		for (i = 0; i < NOPS; i++) {
			/* find op code and execute it */
			if (memcmp(vm.program.lines[vm.pc] + 8, opcodes[i].code, 3) == 0) {
				opcodes[i].fn();
				break;
			}
		}
		/* TODO probably want to abort here if no op code found */
	}

}

int main(int argc, char *argv[]) {

	(void)argc;
	(void)argv;

	load();
	run();

	exit(EXIT_SUCCESS);
}

