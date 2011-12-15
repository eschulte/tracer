/* Global State */
int class;

/* Elf Class */
#define ELF32 1
#define ELF64 2

/* Elf Header */
#define   CLASS(buf) buf[4]
#define    TYPE(buf) int_from_bytes(buf, 16,                       2)
#define MACHINE(buf) int_from_bytes(buf, 18,                       2)
#define VERSION(buf) int_from_bytes(buf, 20,                       4)
#define   ENTRY(buf) int_from_bytes(buf, 24,                       (class * 4))
#define   PHOFF(buf) int_from_bytes(buf, 24 +       (class * 4),   (class * 4))
#define   SHOFF(buf) int_from_bytes(buf, 24 +       (class * 8),   (class * 4))
#define   FLAGS(buf) int_from_bytes(buf, 24 +       (class * 12),  4)
#define   EH_SZ(buf) int_from_bytes(buf, 24 + (4  + (class * 12)), 2)
#define PH_E_SZ(buf) int_from_bytes(buf, 24 + (6  + (class * 12)), 2)
#define  PH_NUM(buf) int_from_bytes(buf, 24 + (8  + (class * 12)), 2)
#define SH_E_SZ(buf) int_from_bytes(buf, 24 + (10 + (class * 12)), 2)
#define  SH_NUM(buf) int_from_bytes(buf, 24 + (12 + (class * 12)), 2)
#define SH_SRND(buf) int_from_bytes(buf, 24 + (14 + (class * 12)), 2)

/* Section Header */
#define SH_NAME(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 0),                          4)
#define SH_ADDR(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 12),               (class * 4))
#define  SH_OFF(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 12 + (class * 4)), (class * 4))
#define SH_SIZE(buf,shoff,shesz,ind) int_from_bytes(buf, (shoff + (shesz * ind) + 12 + (class * 8)), (class * 4))

/* Functions */
void sanity_check(char *buf);
char *read_raw(char *path);
int file_size(char *path);
void write_raw(char *path, char *buf, int size);
int check_magic(char *buf);
int elf_class(char *buf);
unsigned int int_from_bytes(char *buf, int pos, int num);
int print_header_info(char *buf);
int text_section_header(char *buf);
char *section_data(char *buf, int id);
int get_text_address(char *path);
int get_text_offset(char *path);
int get_text_data_size(char *path);
char *get_text_data(char *path);
