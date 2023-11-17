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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static FILE *elf_fp = NULL;
static char* section_header_strtab = NULL; 
static Elf32_Ehdr Header;
static Elf32_Shdr Section;
static Elf32_Shdr shstr_section, symtab_section, strtab_section;
struct symbol{
  char *name;
  uint32_t st,en;
};

void init_elf(const char *elf_file) {
  if (elf_file == NULL)return;

  elf_fp = fopen(elf_file, "rb");
  Assert(elf_fp, "Can not read '%s'", elf_file);
  FILE *fp =elf_fp;

  if(fread(&Header, sizeof(Elf32_Ehdr), 1, fp)!=1){
    Assert(0, "Can not read ELF Header");
  }
  printf("%d,%d,%d\n",Header.e_shoff,Header.e_shentsize,Header.e_shnum);
  Assert(Header.e_shstrndx!=SHN_UNDEF, "There is no String and Symble Table ");

  fseek(fp, Header.e_shoff + Header.e_shstrndx * Header.e_shentsize, SEEK_SET);
  Assert(fread(&shstr_section, sizeof(Elf32_Shdr), 1, fp)==1, "Can not read shstr_section");
  fseek(fp, shstr_section.sh_offset, SEEK_SET);
  section_header_strtab = alloca(sizeof(char)* shstr_section.sh_size +1);
  Assert(fread(section_header_strtab, sizeof(char), shstr_section.sh_size, fp)==shstr_section.sh_size, "Can not read shstr Table");

  fseek(fp, Header.e_shoff, SEEK_SET);
  for(int i=0;i<Header.e_shnum;i++){
    Assert(fread(&Section, sizeof(Elf32_Shdr), 1, fp)==1, "Can not read Section Table");
    if(Section.sh_type == SHT_SYMTAB){
      symtab_section = Section;
    }
    if(Section.sh_type == SHT_STRTAB && strcmp(section_header_strtab+Section.sh_name, ".strtab")==0){
      strtab_section = Section;
      printf("get strtab");
    }
  }
  
  return;
}

bool elf_enable() {
  return elf_fp!=NULL;
}
