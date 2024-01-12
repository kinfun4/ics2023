/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "../local-include/reg.h"
#include <isa.h>
uint32_t csrs[1 << 12];

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  // printf("NO=0x%08x  epc=0x%08x\n",NO,epc);
  CSR(MEPC) = epc;
  CSR(MSTATUS) =
      ((CSR(MSTATUS) & MIE) ? CSR(MSTATUS) | MIPE : CSR(MSTATUS) & (~MIPE)) &
      (~MIE);
  CSR(MCAUSE) = NO;
  return CSR(MTVEC);
}

word_t isa_query_intr() {
  static int cnt = 0;
  if (cpu.intr) {
    cnt++;
    printf("intr = %d, MIE = %d\n", cpu.intr, CSR(MSTATUS) & MIE);
    assert(cnt < 1000);
  }
  if (cpu.intr && CSR(MSTATUS) & MIE) {
    cpu.intr = false;
    return IRQ_TIMER;
  }
  cpu.intr = false;
  return INTR_EMPTY;
}
