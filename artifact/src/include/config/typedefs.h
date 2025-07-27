typedef char i8;
typedef unsigned char byte;
typedef int16_t i16;
typedef uint16_t word;
typedef int32_t i32;
typedef uint32_t quad;
typedef int64_t i64;
typedef i64 time64_t;
typedef uint64_t util;

typedef word cls;
typedef word status;
typedef byte boolean;
typedef byte flags8;
typedef quad flags32;

#define TRUE 1
#define FALSE 0

extern word GLOBAL_flags;
enum global_flags {
    NO_COLOR = 1 << 10,
    HTML_OUTPUT = 1 << 11,
};

typedef struct typehdr {
    cls of;
    status state;
} Type;

typedef struct rangehdr {
    cls of;
    i16 range;
} RangeType;

typedef struct subrangehdr {
    cls of;
    i8 range8;
    i8 normalState;
} SubRangeType;

typedef struct virt  {
    Type type;
} Abstract;

typedef status (*DoFunc)(struct mem_ctx *m, Abstract *a);
typedef boolean (*EqualFunc)(Abstract *a, void *b); /* eq */
typedef Abstract *(*Maker)(struct mem_ctx *m, Abstract *a); /* mk */
typedef status (*SourceFunc)(struct mem_ctx *m, Abstract *a, Abstract *source);
typedef Abstract *(*KeyFunc)(Abstract *a, Abstract *key, Abstract *source);

#define ZERO 0
#define MAX_BASE10 23

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
