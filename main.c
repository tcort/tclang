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

/* number of labels */
#define SYMSZ (1024)

/* number of memory cells in main memory */
#define MEMSZ (32768)

/* number of memory cells in stack */
#define STKSZ (8192)

/* number of memory cells in call stack */
#define CSTKSZ (512)

/* length of line buffer (lines must be 127 chars or less */
#define LNLEN (128)

/* max number of lines */
#define LNMAX (8192) 

/* length of labels */
#define LBLLN (8)

/* basic data type for storage (memory and stack) */
typedef int32_t cell_t;

struct stack {
	cell_t mem[STKSZ];	/* array of memory cells used by this stack */
	size_t sp;		/* stack pointer */
};
typedef struct stack stack_t;

struct call_stack {
	size_t mem[CSTKSZ];	/* array of memory cells used by this stack */
	size_t sp;		/* stack pointer */
};
typedef struct call_stack call_stack_t;

struct symbol {
	char label[LBLLN];	/* label name + '\0' */
	size_t lineno;		/* line on which the label appears */
};
typedef struct symbol symbol_t;

struct symtab {
	symbol_t symbols[SYMSZ];	/* stack of symbols */
	size_t sp;			/* pointer to top of stack */
};
typedef struct symtab symtab_t;

struct program {
	char lines[LNMAX][LNLEN];	/* store whole program text here */
	size_t sp;			/* pointer to last line */
};
typedef struct program program_t;

struct vm {
	cell_t memory[MEMSZ];		/* main memory */
	stack_t stack;			/* working stack */
	call_stack_t call_stack;	/* call stack */
	symtab_t symtab;		/* symbol table */
	program_t program;		/* text of program */
	size_t pc;			/* program counter */
	int done;			/* flag to indicate when to quit */
	char pad[4];
};
typedef struct vm vm_t;

static vm_t vm;

struct operation {
	char code[4];
	char pad[4];
	void (*fn)(void);
};
typedef struct operation op_t;

static cell_t peekstack(void) {
	if (vm.stack.sp == 0) {
		return 0;
	}
	return vm.stack.mem[vm.stack.sp-1];
}

static cell_t popstack(void) {
	if (vm.stack.sp == 0) {
		return 0;
	}
	vm.stack.sp--;
	return vm.stack.mem[vm.stack.sp];
}

static void pushstack(cell_t c) {
	if (vm.stack.sp + 1 >= STKSZ) {
		return;
	}

	vm.stack.mem[vm.stack.sp] = c;
	vm.stack.sp++;
}

static size_t call_return(void) {
	if (vm.call_stack.sp == 0) {
		return 0;
	}
	vm.call_stack.sp--;
	return vm.call_stack.mem[vm.call_stack.sp];
}

static void call_link(size_t c) {
	if (vm.call_stack.sp + 1 >= CSTKSZ) {
		return;
	}

	vm.call_stack.mem[vm.call_stack.sp] = c;
	vm.call_stack.sp++;
}

static size_t symfind(char *label);

static void add(void) { pushstack(popstack() + popstack()); }
static void bra(void) { vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void bnz(void) { if (popstack() != 0) vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void xdiv(void) { pushstack(popstack() / popstack()); }
static void dec(void) { pushstack(popstack() - 1); }
static void dup(void) { int32_t val = popstack(); pushstack(val); pushstack(val); }
static void hlt(void) { vm.done = 1; }
static void inc(void) { pushstack(popstack() + 1); }
static void jal(void) { call_link(vm.pc); vm.pc = symfind(vm.program.lines[vm.pc] + 12); }
static void lda(void) { pushstack(vm.memory[atoi(vm.program.lines[vm.pc] + 12)]); }
static void ldi(void) { pushstack(atoi(vm.program.lines[vm.pc] + 12)); }
static void mod(void) { pushstack(popstack() % popstack()); }
static void mul(void) { pushstack(popstack() * popstack()); }
static void out(void) { fprintf(stdout, "%d\n", popstack()); }
static void prn(void) { fprintf(stdout, "%s\n", vm.program.lines[vm.pc] + 12); }
static void rtn(void) { vm.pc = call_return(); }
static void sta(void) { vm.memory[atoi(vm.program.lines[vm.pc] + 12)] = popstack(); }
static void sub(void) { pushstack(popstack() - popstack()); }

#define NOPS (18)
static op_t opcodes[NOPS] = {
	{ { 'A', 'D', 'D', '\0' }, { 0, 0, 0, 0 }, add },
	{ { 'B', 'R', 'A', '\0' }, { 0, 0, 0, 0 }, bra },
	{ { 'B', 'N', 'Z', '\0' }, { 0, 0, 0, 0 }, bnz },
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

