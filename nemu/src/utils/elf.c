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

#include <alloca.h>
#include <common.h>
#include <elf.h>
#include <stdio.h>

FILE *elf_fp = NULL;

void init_elf(const char *elf_file) {
  if (elf_file == NULL)return;

  FILE *fp = fopen(elf_file, "rb");
  Assert(fp, "Can not read '%s'", elf_file);
  elf_fp = fp;
  Elf32_Ehdr header;
  if(!fread(&header, sizeof(Elf32_Ehdr), 1, elf_fp))return;
  printf("%u,%u\n",header.e_shoff,header.e_phoff);
}

bool elf_enable() {
  return elf_fp!=NULL;
}
