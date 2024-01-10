#include "klib-macros.h"
#include "memory.h"
#include <elf.h>
#include <fs.h>
#include <proc.h>
#include <stddef.h>
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

static size_t page_load(PCB *pcb, int fd, void *vaddr, size_t file_sz, size_t mem_sz) {
  size_t nload = 0;
  uintptr_t offset = READ_LOW((uintptr_t)vaddr, PGWIDTH);
  uintptr_t vpn = READ_HIGH((uintptr_t)vaddr, PGWIDTH);

  while (nload < file_sz) {
    void *_paddr = new_page(1);
    assert(_paddr);
    size_t _len =
        (PGSIZE - offset < file_sz - nload) ? PGSIZE - offset : file_sz - nload;
    void *_vaddr = (void *)vpn;
    map(&pcb->as, _vaddr, _paddr, 0x7);
    printf("paddr = %#x, vaddr = %#x, len = %#x\n", _paddr + offset, _vaddr, _len);
    fs_read(fd, _paddr + offset, _len);
    memset(_paddr + offset + _len, 0, PGSIZE - offset - _len);
    vpn += PGSIZE;
    nload += PGSIZE - offset;
    offset = 0;
  }

  while (nload < mem_sz) {
    void *_paddr = new_page(1);
    assert(_paddr);
    size_t _len =
        (PGSIZE - offset < mem_sz - nload) ? PGSIZE - offset : mem_sz - nload;
    void *_vaddr = (void *)vpn;
    map(&pcb->as, _vaddr, _paddr, 0x7);
    memset(_paddr + offset, 0, _len);
    printf("paddr = %#x, vaddr = %#x, len = %#x\n", _paddr + offset, _vaddr, _len);
    vpn += PGSIZE;
    nload += _len;
    offset = 0;
  }

  return mem_sz;
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
      printf("st = %#x, file_sz =  %#x, mem_sz = %#x\n", Phdr->p_vaddr,
             Phdr->p_filesz, Phdr->p_memsz);
      fs_lseek(fd, Phdr->p_offset, SEEK_SET);
      page_load(pcb, fd, (void *)Phdr->p_vaddr, Phdr->p_filesz, Phdr->p_memsz);
      brk = MAX(brk, ROUNDUP(Phdr->p_vaddr + Phdr->p_memsz, PGSIZE));
    }
  }

  pcb->max_brk = brk;
  fs_close(fd);

  return Ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
