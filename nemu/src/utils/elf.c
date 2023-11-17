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

#include "debug.h"
#include <alloca.h>
#include <common.h>
#include <elf.h>
#include <stdio.h>

FILE *elf_fp = NULL;

void init_elf(const char *elf_file) {
  if (elf_file == NULL)return;

  elf_fp = fopen(elf_file, "rb");
  Assert(elf_fp, "Can not read '%s'", elf_file);
  FILE *fp =elf_fp;

  Elf32_Ehdr header;
  if(fread(&header, sizeof(Elf32_Ehdr), 1, fp)!=1){
    Assert(0, "Can not read ELF Header");
  }
  printf("%d,%d,%d\n",header.e_shoff,header.e_shentsize,header.e_shnum);
  Assert(header.e_shstrndx!=SHN_UNDEF, "There is no String and Symble Table ");

  fseek(fp, header.e_shoff + (header.e_shstrndx-1)*header.e_shentsize, SEEK_SET);
  Elf32_Shdr str_table;
  Assert(fread(&str_table, sizeof(Elf32_Shdr), 1, elf_fp)==1, "Can not read Section Table");
  
  fseek(fp, str_table.sh_offset, SEEK_SET);
  // char * symble = alloca(sizeof(char)*20);
  //   Assert(fscanf(fp, "%s", symble)!=EOF,"Can not read symble!");
  //   printf("%s\n",symble);
  return;
}

bool elf_enable() {
  return elf_fp!=NULL;
}
