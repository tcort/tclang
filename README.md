# tclang

`tclang` is an experimental programming language. The goal is to develop a high level assembly-like language for a stack machine
which also has random access memory and is portable and easily implementable in a number of programming languages. The ideal use
case for such a language is as an intermediate language for a compiler or interpreter. 

## Status

This is early days, and as such, there is a lot of churn. Changes are likely.

## Example

This little demo prints the squares of number from 1 to 10. It shows off the stack, main memory, looping, sub-routines, and output.

```
MAIN
        OTS Squares of integers from 1..10
        LDI 1
        STA 42
        LDI 10
        STA 88
LOOP
        LDA 88
        LDA 42
        CLE
        BEZ DONE
        LDA 42
        JAL SQR
        OTI
        LDA 42
        INC
        STA 42
        BRA LOOP
DONE
        HLT
SQR
        DUP
        MUL
        RTN
```

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

### Bitwise

| code  | operand  | description                                                                                                     |
| ----- | -------- | --------------------------------------------------------------------------------------------------------------- |
| `AND` |          | Pops the top two numbers off of the stack, performs a bitwise AND, and pushes the result onto the stack.        |
| `BLS` |          | Pops the top two numbers off of the stack, performs a binary left shift, and pushes the result onto the stack.  |
| `BRS` |          | Pops the top two numbers off of the stack, performs a binary right shift, and pushes the result onto the stack. |
| `NOT` |          | Pops the top two numbers off of the stack, performs a bitwise NOT, and pushes the result onto the stack.        |
| `OAR` |          | Pops the top two numbers off of the stack, performs a bitwise OR, and pushes the result onto the stack.         |
| `XOR` |          | Pops the top two numbers off of the stack, performs a bitwise XOR, and pushes the result onto the stack.        |

### Flow Control

| code  | operand  | description                                                                                 |
| ----- | -------- | ------------------------------------------------------------------------------------------- |
| `BRA` | label    | Branch always to the label.                                                                 |
| `BEZ` | label    | Pops the top number off of the stack and branches to the label when the number is zero.     |
| `BNZ` | label    | Pops the top number off of the stack and branches to the label when the number is non-zero. |

### Comparisons

| code  | operand | description                                                                                            |
| ----- | ------- | ------------------------------------------------------------------------------------------------------ |
| `CEQ` |         | Pops the top two values of the stack. Pushes 1 onto the stack if equal, else pushes 0.                 |
| `CNE` |         | Pops the top two values of the stack. Pushes 1 onto the stack if not equal, else pushes 0.             |
| `CLE` |         | Pops the top two values of the stack. Pushes 1 onto the stack if less than or equal, else pushes 0.    |
| `CLT` |         | Pops the top two values of the stack. Pushes 1 onto the stack if less than, else pushes 0.             |
| `CGE` |         | Pops the top two values of the stack. Pushes 1 onto the stack if greater than or equal, else pushes 0. |
| `CGT` |         | Pops the top two values of the stack. Pushes 1 onto the stack if greater than, else pushes 0.          |

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
| `OTS` | string  | Prints the string to standard output.                       |
| `OTI` |         | Pops the top of the stack and prints it to standard output. |

### Misc

| code  | operand  | description                     |
| ----- | -------- | ------------------------------- |
| `HLT` |          | Halts execution of the program. |

