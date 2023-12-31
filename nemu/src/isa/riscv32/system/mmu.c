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
#include "common.h"
#include "isa-def.h"
#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <stdint.h>

#define MODE (1 << 31)

int isa_mmu_check(vaddr_t vaddr, int len, int type) {
  uint32_t satp = CSR(SATP);
  if (satp & MODE)
    return MMU_TRANSLATE;
  else
    return MMU_DIRECT;
}

#define READ_HIGH(a, x)     ((a) & (~((1 << (x)) - 1)))
#define READ_LOW(a, x)      ((a) & ((1 << (x)) - 1))
#define GET_PPN(pte) (READ_HIGH((pte), 10) << 2)
#define GET_PTE(pa)  (READ_HIGH((pa), 12) >> 2)

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  uint32_t satp = CSR(SATP);
  uintptr_t ptr = READ_LOW(satp, 22) << 12;
  uintptr_t vpn1 = (uintptr_t)vaddr >> 22;
  uintptr_t vpn0 = BITS((uintptr_t)vaddr, 21, 12); 
  PTE *pte1 = (void *)ptr + vpn1 * PTESIZE;
  assert(*pte1 & PTE_V);
  PTE* pte0 = (void *)GET_PPN((uintptr_t)*pte1) + vpn0 * PTESIZE;
  assert(*pte0 & PTE_V);
  paddr_t paddr = GET_PPN((uintptr_t)*pte0) + READ_LOW(vaddr, 12);
  return paddr;
}

paddr_t isa_mmu_execute(vaddr_t vaddr, int len, int type) {
  paddr_t addr = 0;
  switch (isa_mmu_check(vaddr, len, type)) {
  case MMU_DIRECT:
    addr = vaddr;
    break;
  case MMU_TRANSLATE:
    addr = isa_mmu_translate(vaddr, len, type);
    break;
  case MMU_FAIL:
    panic("Invalid address!");
  }
  return addr;
}
