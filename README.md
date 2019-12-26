# tclang

experimental programming language.

## Syntax

* comment - begins with an octothorp (`#`). Matches `^#.*$`.
* label - up to 7 characters long and may not begin with a blank (` `) nor an octothorp (`#`). Matches `^[^ #]{1,7}`.
* opcode - 3 upper case letters. Matches `[A-Z]{3}`.
* operand - dependent on opcode.

## Grammar

```
program	: lines
	;

lines	: lines line
	| line
	;

line	: comment
	| code
	;

comment	: '#' [anything]

code	: opcode operand
	;
```

## Source Code Format

Lines of source code are fixed length records.

* Position 1 through 7 - label or blanks (` `) or `#` in position 1 for a comment line.
* Position 8 - blank (` `)
* Position 9 through 11 - opcode.
* Position 12 - blank (` `)
* Position 13 through 72 - operand.

## Environment

* A memory cell may hold a 32 bit signed integer.
* There are 32,768 random access memory cells.
* There is a stack with 8,192 memory cells.

## Virtual Machine Description

The virtual machine has a stack and main memory. Opcodes operate on the stack and
accept 0 or 1 operands with effect the stack. For example, `LDI 2` pushes `2` onto
the stack. `ADD` pops the top two numbers off of the stack, sums them, and pushes
the result onto the stack. Values can be moved between the stack and main memory.

## opcodes

### Arithmetic

| code  | operand              | description                                                                                       |
| ----- | -------------------- | ------------------------------------------------------------------------------------------------- |
| `ADD` | *none*               | Pops the top two numbers off of the stack, adds them, and pushes the result onto the stack.       |
| `SUB` | *none*               | Pops the top two numbers off of the stack, subtracts them, and pushes the result onto the stack.  |
| `MUL` | *none*               | Pops the top two numbers off of the stack, multiplies them, and pushes the result onto the stack. |
| `DIV` | *none*               | Pops the top two numbers off of the stack, divides them, and pushes the result onto the stack.    |
| `MOD` | *none*               | Pops the top two numbers off of the stack, divides them, and pushes the remained onto the stack.  |

### Flow Control

| code  | operand  | description                                                           |
| ----- | -------- | --------------------------------------------------------------------- |
| `JMP` | label    | Unconditionally jumps to the line which contains the specified label. |

### Data Load / Store

| code  | operand  | description                                             |
| ----- | -------- | ------------------------------------------------------- |
| `LDI` | number   | Loads the immediate value and pushes it onto the stack. |

### Input / Output

| code  | operand | description                                                 |
| ----- | ------- | ----------------------------------------------------------- |
| `PRN` | string  | Prints the string to standard output.                       |
| `OUT` | *none*  | Pops the top of the stack and prints it to standard output. |

### Misc

| code  | operand  | description                     |
| ----- | -------- | ------------------------------- |
| `HLT` | *none*   | Halts execution of the program. |

