#include "klib-macros.h"
#include "memory.h"
#include <elf.h>
#include <fs.h>
#include <proc.h>
#include <stdint.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
#define EXPECT_TYPE EM_RISCV
#else
#define EXPECT_TYPE EM_NONE
#endif

static size_t page_load(PCB *pcb, int fd, void *vaddr, size_t len) {
  size_t nload = 0;
  while (nload < len) {
    void *paddr = new_page(1);
    assert(paddr);
    size_t _len = (PGSIZE < len - nload) ? PGSIZE : len - nload;
    void *_vaddr = vaddr + nload;
    map(&pcb->as, _vaddr, paddr, 0x7);
    fs_read(fd, paddr, _len);
    memset(paddr, 0, PGSIZE - _len);
    nload += _len;
  }
  return len;
}

static size_t page_clear(PCB *pcb, void *vaddr, size_t len) {
  size_t nload = 0;
  while (nload < len) {
    void *paddr = new_page(1);
    assert(paddr);
    size_t _len = (PGSIZE < len - nload) ? PGSIZE : len - nload;
    void *_vaddr = vaddr + nload;
    map(&pcb->as, _vaddr, paddr, 0x7);
    memset(paddr, 0, _len);
    nload += _len;
  }
  return len;
}

uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  uintptr_t brk = 0; 

  Elf_Ehdr Ehdr[1];
  fs_read(fd, Ehdr, sizeof(Elf_Ehdr));

  assert(Ehdr->e_ident[EI_MAG0] == ELFMAG0);
  assert(Ehdr->e_ident[EI_MAG1] == ELFMAG1);
  assert(Ehdr->e_ident[EI_MAG2] == ELFMAG2);
  assert(Ehdr->e_ident[EI_MAG3] == ELFMAG3);
  assert(Ehdr->e_machine == EXPECT_TYPE);
  assert(Ehdr->e_phoff != 0 && Ehdr->e_phnum != 0);

  Elf_Phdr Phdr[1];
  for (int i = 0; i < Ehdr->e_phnum; i++) {
    fs_lseek(fd, Ehdr->e_phoff + i * Ehdr->e_phentsize, SEEK_SET);
    fs_read(fd, Phdr, Ehdr->e_phentsize);

    if (Phdr->p_type == PT_LOAD) {
      fs_lseek(fd, Phdr->p_offset, SEEK_SET);
      page_load(pcb, fd, (void *)Phdr->p_vaddr, Phdr->p_filesz);
      page_clear(pcb, (void *)ROUNDUP(Phdr->p_vaddr + Phdr->p_filesz, PGSIZE),
                 Phdr->p_memsz - Phdr->p_filesz);
      brk = MAX(brk, ROUNDUP(Phdr->p_vaddr + Phdr->p_memsz, PGSIZE));
    }
  }

  pcb->max_brk = brk;
  printf("max_brk = %#x\n", pcb->max_brk);
  fs_close(fd);

  return Ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
