#include <elf.h>
#include <proc.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf32_Ehdr Ehdr[1];
  ramdisk_read(Ehdr, 0, sizeof(Elf32_Ehdr));

  assert(Ehdr[0].e_ident[EI_MAG0] == ELFMAG0);
  assert(Ehdr[0].e_ident[EI_MAG1] == ELFMAG1);
  assert(Ehdr[0].e_ident[EI_MAG2] == ELFMAG2);
  assert(Ehdr[0].e_ident[EI_MAG3] == ELFMAG3);

  Elf32_Addr e_entry = Ehdr[0].e_entry;
  Elf32_Off e_phoff = Ehdr[0].e_phoff;
  uint16_t e_phnum = Ehdr[0].e_phnum;
  uint16_t e_phentsize = Ehdr[0].e_phentsize;

  assert(e_phoff != 0 && e_phnum != 0);

  Elf32_Phdr Phdr[1];
  for (int i = 0; i < e_phnum; i++) {
    ramdisk_read(Phdr, e_phoff + i * e_phentsize, e_phentsize);

    if (Phdr[0].p_type == PT_LOAD) {
      Elf32_Off p_offset = Phdr[0].p_offset;
      Elf32_Addr p_vaddr = Phdr[0].p_vaddr;
      uint32_t p_filesz = Phdr[0].p_filesz;
      uint32_t p_memsz = Phdr[0].p_memsz;
      char buf[p_filesz];

      ramdisk_read(buf, p_offset, p_filesz);
      memcpy((void *)p_vaddr, buf, p_filesz);

      if (p_memsz > p_filesz)
        memset((void *)p_vaddr + p_filesz, 0, p_memsz - p_filesz);
    }
  }

  return e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
