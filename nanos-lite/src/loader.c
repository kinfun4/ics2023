#include <elf.h>
#include <fs.h>
#include <proc.h>
#include <stdlib.h>

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

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
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
      char *buf = malloc(Phdr->p_filesz);
      fs_lseek(fd, Phdr->p_offset, SEEK_SET);
      fs_read(fd, buf, Phdr->p_filesz);
      memcpy((void *)Phdr->p_vaddr, buf, Phdr->p_filesz);
      if (Phdr->p_memsz > Phdr->p_filesz)
        memset((void *)Phdr->p_vaddr + Phdr->p_filesz, 0,
               Phdr->p_memsz - Phdr->p_filesz);
    }
  }
  fs_close(fd);

  return Ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
