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

#include "debug.h"
#include <alloca.h>
#include <common.h>
#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PRINT_FUNC(type, pc , depth, name, dnpc) \
do{\
  printf("0x%08x: ", pc); \
  for (int j = 0; j < depth; j++) \
    printf(" "); \
  printf("%-4s [%s@0x%08x]\n", type, name, dnpc);\
} while(0)

static FILE *elf_fp = NULL;
static char *shstr_tab;
static char *str_tab;
static Elf32_Ehdr Header;
static Elf32_Shdr Section;
static Elf32_Shdr shstr_section, symtab_section, strtab_section;
static Elf32_Sym Symbol;
static uint32_t symtab_ndx, strtab_ndx, symtab_num;
static uint32_t func_cnt;
static int depth;
static int stack[2000];
struct func {
  char *name;
  word_t st, en;
} static func_tab[5000];

void init_elf(char **elf_file, int elf_cnt) {
#ifndef CONFIG_FTRACE
  return;
#endif /* ifndef CONFIG_FTRACE */
  while (elf_cnt > 0) {
    char *elf = elf_file[--elf_cnt];

    if (elf == NULL)
      return;

    elf_fp = fopen(elf, "rb");
    Assert(elf_fp, "Can not read '%s'", elf);
    FILE *fp = elf_fp;

    Assert(fread(&Header, sizeof(Elf32_Ehdr), 1, fp) == 1,
           "Can not read ELF Header");
    Assert(Header.e_shstrndx != SHN_UNDEF,
           "There is no String and Symble Table ");

    // get section header string table
    fseek(fp, Header.e_shoff + Header.e_shstrndx * Header.e_shentsize,
          SEEK_SET);
    Assert(fread(&shstr_section, sizeof(Elf32_Shdr), 1, fp) == 1,
           "Can not read shstr_section");
    fseek(fp, shstr_section.sh_offset, SEEK_SET);
    shstr_tab = malloc(sizeof(char) * shstr_section.sh_size);
    Assert(fread(shstr_tab, sizeof(char), shstr_section.sh_size, fp) ==
               shstr_section.sh_size,
           "Can not read shstr Table");

    fseek(fp, Header.e_shoff, SEEK_SET);
    for (int i = 0; i < Header.e_shnum; i++) {
      Assert(fread(&Section, sizeof(Elf32_Shdr), 1, fp) == 1,
             "Can not read Section Table");
      if (Section.sh_type == SHT_SYMTAB &&
          strcmp(shstr_tab + Section.sh_name, ".symtab") == 0) {
        symtab_section = Section;
        symtab_ndx = i;
        symtab_num = Section.sh_size / Section.sh_entsize;
      }
      if (Section.sh_type == SHT_STRTAB &&
          strcmp(shstr_tab + Section.sh_name, ".strtab") == 0) {
        strtab_section = Section;
        strtab_ndx = i;
      }
    }
    Assert(symtab_ndx != 0, "Can not get symtab_section");
    Assert(strtab_ndx != 0, "Can not get strtab_section");

    fseek(fp, strtab_section.sh_offset, SEEK_SET);
    str_tab = malloc(sizeof(char) * strtab_section.sh_size);
    Assert(fread(str_tab, sizeof(char), strtab_section.sh_size, fp) ==
               strtab_section.sh_size,
           "Can not read str Table");

    fseek(fp, symtab_section.sh_offset, SEEK_SET);
    for (int i = 0; i < symtab_num; i++) {
      Assert(fread(&Symbol, sizeof(Elf32_Sym), 1, fp) == 1,
             "Can not read symbol Table");
      if (ELF32_ST_TYPE(Symbol.st_info) == STT_FUNC) {
        func_tab[func_cnt].name = malloc(sizeof(char) * 80);
        strcpy(func_tab[func_cnt].name, str_tab + Symbol.st_name);
        func_tab[func_cnt].st = Symbol.st_value;
        func_tab[func_cnt].en = Symbol.st_value + Symbol.st_size;
        // printf("name = %30s, st = %#x, en = %#x\n", func_tab[func_cnt].name,
        //        func_tab[func_cnt].st, func_tab[func_cnt].en);
        func_cnt++;
      }
    }
    free(shstr_tab);
    free(str_tab);
  }
  return;
}

int find_func(word_t pc) {
  for (int i = 0; i < func_cnt; i++) {
    if (func_tab[i].st <= pc && pc < func_tab[i].en) {
      return i;
    }
  }
  return -1;
}

void func_call(word_t pc, word_t dnpc) {
#ifndef CONFIG_FTRACE
  return;
#endif /* ifndef CONFIG_FTRACE */
  if (elf_fp == NULL)return;

  int func1 = find_func(pc);
  int func2 = find_func(dnpc);
  Assert(func1 != -1 && func2 != -1, "pc = %#x, func1 = %d, dnpc = %#x, func2 = %d\n", pc, func1, dnpc, func2);

  if (dnpc == func_tab[func2].st) {
    PRINT_FUNC("call", pc, 2, func_tab[func2].name, dnpc);
    stack[depth++] = func1;
  }
}



void func_ret(word_t pc, word_t dnpc) {
#ifndef CONFIG_FTRACE
  return;
#endif /* ifndef CONFIG_FTRACE */
  if (elf_fp == NULL)return;

  int func1 = find_func(pc);
  int func2 = find_func(dnpc);
  Assert(func1 != -1 && func2 != -1, "pc = %#x, func1 = %d, dnpc = %#x, func2 = %d\n", pc, func1, dnpc, func2);

  PRINT_FUNC("cur", pc, 2, func_tab[func1].name, pc);

  // while (stack[--depth] != func2) {
  //   PRINT_FUNC("ret", pc, depth, func_tab[stack[depth]].name, func_tab[stack[depth]].en);
  //   Assert(depth >= 0, "Out of bound! func name = %20s, pc = %#x", func_tab[func2].name, dnpc);
  // }

  PRINT_FUNC("ret", pc, 2, func_tab[func2].name, dnpc);
}
