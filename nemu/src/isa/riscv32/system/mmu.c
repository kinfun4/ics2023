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
#include <assert.h>
#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <stdint.h>

#define MODE (1 << 31)
#define READ_HIGH(a, x)     ((a) & (~((1 << (x)) - 1)))
#define READ_LOW(a, x)      ((a) & ((1 << (x)) - 1))
#define GET_PPN(pte) (READ_HIGH((pte), 10) << 2)
#define GET_PTE(pa)  (READ_HIGH((pa), 12) >> 2)

int isa_mmu_check(vaddr_t vaddr, int len, int type) {
  uint32_t satp = CSR(SATP);
  if (satp & MODE)
    return MMU_TRANSLATE;
  else
    return MMU_DIRECT;
}

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  uint32_t satp = CSR(SATP);
  paddr_t ptr = READ_LOW(satp, 22) << 12;
  paddr_t vpn1 = vaddr >> 22;
  paddr_t vpn0 = BITS(vaddr, 21, 12); 
  printf("vaddr = %#x\n", vaddr);
  // printf("ptr = %#x\n", ptr);
  // printf("vpn1 = %#x\n", vpn1);
  paddr_t pte1_ptr = ptr + vpn1 * PTESIZE;
  // printf("&pte1 = %#x\n", pte1_ptr);
  PTE pte1 = paddr_read(pte1_ptr, PTESIZE);
  // printf("pte1 = %#x\n", pte1);
  assert(pte1 & PTE_V);
  paddr_t pte0_ptr = GET_PPN(pte1) + vpn0 * PTESIZE;
  // printf("&pte0_ptr = %#x\n", pte0_ptr);
  PTE pte0 = paddr_read(pte0_ptr, PTESIZE);
  assert(pte0 & PTE_V);
  paddr_t paddr = GET_PPN(pte0) + READ_LOW(vaddr, 12);
  // printf("paddr = %#x\n", paddr);
  assert(paddr == vaddr);
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
  default:
    panic("Invalid address!");
  }
  return addr;
}
