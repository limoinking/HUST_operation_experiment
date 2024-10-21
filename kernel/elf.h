#ifndef _ELF_H_
#define _ELF_H_

#include "util/types.h"
#include "process.h"

#define MAX_CMDLINE_ARGS 64

// elf header structure
typedef struct elf_header_t {
  uint32 magic;
  uint8 elf[12];
  uint16 type;      /* Object file type */
  uint16 machine;   /* Architecture */
  uint32 version;   /* Object file version */
  uint64 entry;     /* Entry point virtual address */
  uint64 phoff;     /* Program header table file offset */
  uint64 shoff;     /* Section header table file offset */
  uint32 flags;     /* Processor-specific flags */
  uint16 ehsize;    /* ELF header size in bytes */
  uint16 phentsize; /* Program header table entry size */
  uint16 phnum;     /* Program header table entry count */
  uint16 shentsize; /* Section header table entry size */
  uint16 shnum;     /* Section header table entry count */
  uint16 shstrndx;  /* Section header string table index */
} elf_header;


typedef struct elf_section_header {
  uint32 sh_name;      /* 段名称（字符串表索引） */
  uint32 sh_type;      /* 段类型 */
  uint64 sh_flags;     /* 段标志 */
  uint64 sh_addr;      /* 段的虚拟地址（执行时） */
  uint64 sh_offset;    /* 段在文件中的偏移 */
  uint64 sh_size;      /* 段的大小（以字节为单位） */
  uint32 sh_link;      /* 链接到另一个段 */
  uint32 sh_info;      /* 额外的段信息 */
  uint64 sh_addralign; /* 段的对齐要求 */
  uint64 sh_entsize;   /* 如果段保存表格，则表示表项大小 */
} elf_section_header;



// Program segment header.
typedef struct elf_prog_header_t {
  uint32 type;   /* Segment type */
  uint32 flags;  /* Segment flags */
  uint64 off;    /* Segment file offset */
  uint64 vaddr;  /* Segment virtual address */
  uint64 paddr;  /* Segment physical address */
  uint64 filesz; /* Segment size in file */
  uint64 memsz;  /* Segment size in memory */
  uint64 align;  /* Segment alignment */
} elf_prog_header;

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian
#define ELF_PROG_LOAD 1

// #define SHT_SYMTAB 2
// #define SHT_STRTAB 3
// #define STT_FILE 4
// #define STT_FUNC 18

typedef enum elf_status_t {
  EL_OK = 0,

  EL_EIO,
  EL_ENOMEM,
  EL_NOTELF,
  EL_ERR,

} elf_status;

typedef struct elf_symbol {
  // st_name 是字符串表中的偏移量。
  uint32 st_name;         /* 符号名称（字符串表索引） */
  unsigned char st_info;  /* 符号类型和绑定信息 */
  unsigned char st_other; /* 符号可见性 */
  uint16 st_shndx;       /* 段索引 */
  uint64 st_value;       /* 符号值 */
  uint64 st_size;        /* 符号大小 */
} elf_symbol;



// typedef struct elf_ctx_t {
//   void *info;
//   elf_header ehdr;
// } elf_ctx;

typedef struct elf_ctx_t {
  void *info;
  elf_header ehdr;
  char strtable[4096];
  elf_symbol symbols[256];
  uint64 symbols_count;
} elf_ctx;

elf_status elf_init(elf_ctx *ctx, void *info);
elf_status elf_load(elf_ctx *ctx);

elf_status elf_load_symbol(elf_ctx *ctx);
extern elf_ctx elfsymbol;

void load_bincode_from_host_elf(process *p);

#endif
