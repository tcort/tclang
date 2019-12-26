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

line	: comment '\n'
	| code '\n'
	;

comment	: '#' [anything]

code	: label opcode operand
	| opcode operand
	| opcode
	| label
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
* There is a call stack with 512 memory cells.

## Virtual Machine Description

The virtual machine has a stack and main memory. Opcodes operate on the stack and
accept 0 or 1 operands with effect the stack. For example, `LDI 2` pushes `2` onto
the stack. `ADD` pops the top two numbers off of the stack, sums them, and pushes
the result onto the stack. Values can be moved between the stack and main memory.

## opcodes

### Arithmetic

| code  | operand              | description                                                                                       |
| ----- | -------------------- | ------------------------------------------------------------------------------------------------- |
| `ADD` |                      | Pops the top two numbers off of the stack, adds them, and pushes the result onto the stack.       |
| `SUB` |                      | Pops the top two numbers off of the stack, subtracts them, and pushes the result onto the stack.  |
| `MUL` |                      | Pops the top two numbers off of the stack, multiplies them, and pushes the result onto the stack. |
| `DIV` |                      | Pops the top two numbers off of the stack, divides them, and pushes the result onto the stack.    |
| `MOD` |                      | Pops the top two numbers off of the stack, divides them, and pushes the remained onto the stack.  |
| `INC` |                      | Pops the top number off of the stack, adds 1, and pushes the result onto the stack.               |
| `DEC` |                      | Pops the top number off of the stack, subtracts 1, and pushes the result onto the stack.          |

### Flow Control

| code  | operand  | description                                                                                 |
| ----- | -------- | ------------------------------------------------------------------------------------------- |
| `BRA` | label    | Branch always to the label.                                                                 |
| `BNZ` | label    | Pops the top number off of the stack and branches to the label when the number is non-zero. |

### Sub-routines

| code  | operand  | description                                              |
| ----- | -------- | -------------------------------------------------------- |
| `JAL` | label    | Call the sub-routine identified by label.                |
| `RTN` |          | Return to the location where the sub-routine was called. |

### Stack Manipulation

| code  | operand | description                                   |
| ----- | ------- | --------------------------------------------- |
| `DUP` |         | duplicates the value at the top of the stack. |

### Data Load / Store

| code  | operand  | description                                                       |
| ----- | -------- | ----------------------------------------------------------------- |
| `LDI` | number   | Loads the immediate value and pushes it onto the stack.           |
| `LDA` | address  | Loads a value from the given memory address (hex) onto the stack. |
| `STA` | address  | Stores a value to the given memory address (hex) from the stack.  |

### Input / Output

| code  | operand | description                                                 |
| ----- | ------- | ----------------------------------------------------------- |
| `PRN` | string  | Prints the string to standard output.                       |
| `OUT` |         | Pops the top of the stack and prints it to standard output. |

### Misc

| code  | operand  | description                     |
| ----- | -------- | ------------------------------- |
| `HLT` |          | Halts execution of the program. |

