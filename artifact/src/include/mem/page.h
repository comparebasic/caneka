/* 16mb or  4096x4096 */
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define PAGE_COUNT 256
#define PAGE_MAX (256-1)
#define CHAPTER_SIZE (sysconf(_SC_PAGE_SIZE)*PAGE_COUNT)
