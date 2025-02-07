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

struct string;
struct serve_ctx;
struct req;
struct handler;
struct mem_ctx;
struct mem_slab;
struct strcursor_range;
struct span;
struct span_query;
struct span_slab;
struct structexp;
struct lookup;
struct strcursor;
struct strcursor_range;
struct guard;
struct roebling;
struct span_def;
struct mem_handle;
struct mem_keyed;
struct oset;
struct oset_def;
struct access;
struct ioctx;
struct mem_local;
struct test_set;
struct strcursor;
struct cash;

struct format_def;
struct formatter;

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
#define SLAB_START_SIZE 2

#define MAX_DIMS 16

#define STRING_SEG_FOOTPRINT 256
#define STRING_EXTRAS (sizeof(Type)+sizeof(int)+sizeof(struct string *)+sizeof(MemCtx *))
#define STRING_CHUNK_SIZE ((STRING_SEG_FOOTPRINT - STRING_EXTRAS)-1)
#define STRING_FIXED_SIZE ((64  - (sizeof(struct typehdr)+sizeof(int)))-1)
#define STRING_FULL_SIZE 127

#define as(x, t) ((x) != NULL && ((Abstract *)(x))->type.of == (t) ? x : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfc(x, ifc) (((x) != NULL  && Ifc_Match(((Abstract *)(x))->type.of, ifc)) ? (x) : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
