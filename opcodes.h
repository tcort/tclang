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

#ifndef __OPCODES_H
#define __OPCODES_H

#include "types.h"

void op_add(vm_t *vm);
void op_and(vm_t *vm);
void op_bez(vm_t *vm);
void op_bnz(vm_t *vm);
void op_bra(vm_t *vm);
void op_bls(vm_t *vm);
void op_brs(vm_t *vm);
void op_ceq(vm_t *vm);
void op_cge(vm_t *vm);
void op_cgt(vm_t *vm);
void op_cle(vm_t *vm);
void op_clt(vm_t *vm);
void op_cne(vm_t *vm);
void op_dec(vm_t *vm);
void op_div(vm_t *vm);
void op_dup(vm_t *vm);
void op_hlt(vm_t *vm);
void op_inc(vm_t *vm);
void op_ini(vm_t *vm);
void op_jal(vm_t *vm);
void op_lda(vm_t *vm);
void op_ldi(vm_t *vm);
void op_mod(vm_t *vm);
void op_mul(vm_t *vm);
void op_not(vm_t *vm);
void op_oar(vm_t *vm);
void op_oti(vm_t *vm);
void op_ots(vm_t *vm);
void op_rtn(vm_t *vm);
void op_sta(vm_t *vm);
void op_sub(vm_t *vm);
void op_xor(vm_t *vm);

#endif

