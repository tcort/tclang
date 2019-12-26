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

#ifndef __TYPES_H
#define __TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "const.h"

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

struct operation {
	char code[4];
	char pad[4];
	void (*fn)(void);
};
typedef struct operation op_t;

#endif
