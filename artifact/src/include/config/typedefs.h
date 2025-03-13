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

typedef void *slab[SPAN_STRIDE];

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

typedef struct virt  {
    Type type;
} Abstract;

typedef struct reserve  {
    Type type;
    i32 nunits;
} Reserve;

extern Abstract Reserved;

typedef struct virt * AbstractPtr;
typedef status (*DoFunc)(struct mem_ctx *m, Abstract *a);
typedef status (*DblFunc)(struct mem_ctx *m, Abstract *a, Abstract *b);
typedef status (*OutFunc)(struct mem_ctx *m, struct string *s, Abstract *source);
typedef boolean (*EqualFunc)(Abstract *a, void *b); /* eq */
typedef char *(*RangeToChars)(word);

#define ZERO 0
#define MAX_BASE10 23

#define STRING_SEG_FOOTPRINT 256
#define STRING_EXTRAS (sizeof(Type)+sizeof(int)+sizeof(struct string *)+sizeof(MemCtx *))
#define STRING_CHUNK_SIZE 255
#define STRING_FIXED_SIZE 63
#define STRING_FULL_SIZE 127

#define as(x, t) ((x) != NULL && ((Abstract *)(x))->type.of == (t) ? x : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfcOffset(x, ifc, offset) (((x) != NULL  && Ifc_Match((((Abstract *)(x))->type.of-HTYPE_LOCAL), ifc)) ? (x) : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfc(x, ifc) (((x) != NULL  && Ifc_Match(((Abstract *)(x))->type.of, ifc)) ? (x) : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
