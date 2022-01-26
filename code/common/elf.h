//
//  elf.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/6.
//

// 命名尽量与xv6保持一致, 后续使用CONFIG去控制RV32, RV64, RV128
// 有关ELF参考risc-v手册
// https://github.com/riscv/riscv-elf-psabi-doc/.
// 后续参考:http://www.sco.com/developers/gabi/latest/contents.html

//  64-Bit Data Types

//  Name            Size    Alignment   Purpose
//  Elf64_Addr      8       8           Unsigned program address
//  Elf64_Off       8       8           Unsigned file offset
//  Elf64_Half      2       2           Unsigned medium integer
//  Elf64_Word      4       4           Unsigned integer
//  Elf64_Sword     4       4           Signed integer
//  Elf64_Xword     8       8           Unsigned long integer
//  Elf64_Sxword    8       8           Signed long integer
//  unsigned char   1       1           Unsigned small integer

//  ELF Header

//  #define EI_NIDENT 16
//
//  typedef struct {
//        unsigned char   e_ident[EI_NIDENT];
//        Elf64_Half      e_type;
//        Elf64_Half      e_machine;
//        Elf64_Word      e_version;
//        Elf64_Addr      e_entry;
//        Elf64_Off       e_phoff;
//        Elf64_Off       e_shoff;
//        Elf64_Word      e_flags;
//        Elf64_Half      e_ehsize;
//        Elf64_Half      e_phentsize;
//        Elf64_Half      e_phnum;
//        Elf64_Half      e_shentsize;
//        Elf64_Half      e_shnum;
//        Elf64_Half      e_shstrndx;
//  } Elf64_Ehdr;

//  Program Header

//  typedef struct {
//    Elf64_Word    p_type;
//    Elf64_Word    p_flags;
//    Elf64_Off     p_offset;
//    Elf64_Addr    p_vaddr;
//    Elf64_Addr    p_paddr;
//    Elf64_Xword   p_filesz;
//    Elf64_Xword   p_memsz;
//    Elf64_Xword   p_align;
//  } Elf64_Phdr;


#ifndef elf_h
#define elf_h

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" 用小端序表示

struct elfhdr {
    // unsigned char   e_ident[EI_NIDENT];
    // 做了一些改动, 因为暂时只需要去读Magic Number
    unsigned int        magic;
    unsigned int        remaining_identifier[3];
    
    unsigned short      type;
    unsigned short      machine;
    unsigned int        version;
    unsigned long       entry;
    unsigned long       phoff;
    unsigned long       shoff;
    unsigned int        flags;
    unsigned short      ehsize;
    unsigned short      phentsize;
    unsigned short      phnum;
    unsigned short      shentsize;
    unsigned short      shnum;
    unsigned short      shstrndx;
}__attribute__((packed));   // Elf64_Elfhdr

struct proghdr {
    unsigned int    type;
    unsigned int    flags;
    unsigned long   off;    //unsigned long   offset;   后续应该更改为符合现代elf标准
    unsigned long   vaddr;
    unsigned long   paddr;
    unsigned long   filesz;
    unsigned long   memsz;
    unsigned long   align;
}__attribute__((packed));    // Elf64_Proghdr

// Values for Proghdr type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

#endif /* elf_h */
