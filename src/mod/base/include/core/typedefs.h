typedef signed char i8;
typedef unsigned char byte;
typedef int16_t i16;
typedef uint16_t word;
typedef int32_t i32;
typedef uint32_t quad;
typedef int64_t i64;
typedef uint64_t util;

typedef word cls;
typedef word status;
typedef byte boolean;
typedef byte flags8;
typedef word flags16;
typedef quad flags32;

typedef struct typehdr {
    cls of;
    status state;
} Type;

typedef struct rangehdr {
    cls of;
    i16 range;
} RangeType;

typedef struct virt  {
    Type type;
} Abstract;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
