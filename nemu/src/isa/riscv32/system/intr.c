/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
extern int csrs[];
#define MSTATUS 0x300
#define MTVEC 0x305
#define MEPC 0x341
#define MCAUSE 0x342
#define CSR(i) (csrs[i])
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  CSR(MEPC) = epc;
  CSR(MCAUSE) = NO;
  return CSR(MTVEC);
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
