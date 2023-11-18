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
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {
  // 0x00000297,  // auipc t0,0
  // 0x00028823,  // sb  zero,16(t0)
  // 0x0102c503,  // lbu a0,16(t0)
  // 0x00100073,  // ebreak (used as nemu_trap)
  // 0xdeadbeef,  // some data
0x00000093,
0x00000113,
0x00000193,
0x00000213,
0x00000293,
0x00000313,
0x00000393,
0x00000413,
0x00000493,
0x00000513,
0x00000593,
0x00000613,
0x00000693,
0x00000713,
0x00000793,
0x00000813,
0x00000893,
0x00000913,
0x00000993,
0x00000A13,
0x00000A93,
0x00000B13,
0x00000B93,
0x00000C13,
0x00000C93,
0x00000D13,
0x00000D93,
0x00000E13,
0x00000E93,
0x00000F13,
0x00000F93,
0x00C10537,
0xFEE50513,
0xDEAD10CC,
0xC0001073,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = RESET_VECTOR;

  /* The zero register is always 0. */
  cpu.gpr[0] = 0;
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

  /* Initialize this virtual computer system. */
  restart();
}
