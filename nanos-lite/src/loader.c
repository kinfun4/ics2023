#include <elf.h>
#include <proc.h>

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

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr Ehdr[1];
  ramdisk_read(Ehdr, 0, sizeof(Elf_Ehdr));

  assert(Ehdr->e_ident[EI_MAG0] == ELFMAG0);
  assert(Ehdr->e_ident[EI_MAG1] == ELFMAG1);
  assert(Ehdr->e_ident[EI_MAG2] == ELFMAG2);
  assert(Ehdr->e_ident[EI_MAG3] == ELFMAG3);
  assert(Ehdr->e_machine == EXPECT_TYPE);
  assert(Ehdr->e_phoff != 0 && Ehdr->e_phnum != 0);

  Elf_Phdr Phdr[1];
  for (int i = 0; i < Ehdr->e_phnum; i++) {
    ramdisk_read(Phdr, Ehdr->e_phoff + i * Ehdr->e_phentsize,
                 Ehdr->e_phentsize);

    if (Phdr->p_type == PT_LOAD) {
      char buf[Phdr->p_filesz];
      ramdisk_read(buf, Phdr->p_offset, Phdr->p_filesz);
      memcpy((void *)Phdr->p_vaddr, buf, Phdr->p_filesz);
      if (Phdr->p_memsz > Phdr->p_filesz)
        memset((void *)Phdr->p_vaddr + Phdr->p_filesz, 0,
               Phdr->p_memsz - Phdr->p_filesz);
    }
  }

  return Ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
