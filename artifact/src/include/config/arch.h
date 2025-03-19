/* 16mb or  4096x4096 */
#ifndef __ARCH_HEADER_amd64
#define __ARCH_HEADER_amd64
const size_t PAGE_SIZE = 4096;
const size_t PAGE_COUNT = 256;
const size_t PAGE_MAX = (256-1);
const size_t CHAPTER_SIZE = PAGE_SIZE*PAGE_COUNT;
const size_t CHAPTER_MAX = 16;
const size_t MEM_SLAB_SIZE = PAGE_SIZE;
const size_t SPAN_MAX_DIMS = 5; /* 1,048,576 total items @ 16 slots */
const int _increments[SPAN_MAX_DIMS] = {1, 16, 256, 4096, 65536};
const size_t SPAN_STRIDE = 16;
const size_t SPAN_LOCAL_MAX = (SPAN_STRIDE-1);
const size_t TABLE_MAX_DIMS = SPAN_MAX_DIMS;
const size_t STR_MAX = MEM_SLAB_SIZE;
const size_t STR_DEFAULT = 512;
const size_t READ_MAX = STR_DEFAULT;
const size_t WRITE_MAX = STR_DEFAULT;
#endif
