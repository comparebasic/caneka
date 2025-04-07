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

typedef struct virt  {
    Type type;
} Abstract;

typedef status (*DoFunc)(struct mem_ctx *m, Abstract *a);
typedef boolean (*EqualFunc)(Abstract *a, void *b); /* eq */

#define ZERO 0
#define MAX_BASE10 23

#define as(x, t) ((x) != NULL && ((Abstract *)(x))->type.of == (t) ? x : Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from abstract mismatch _O", (i32)(x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfcOffset(x, ifc, offset) (((x) != NULL  && Ifc_Match((((Abstract *)(x))->type.of-HTYPE_LOCAL), ifc)) ? (x) : Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from abstract mismatch _i4", (i32)(x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfc(x, ifc) (((x) != NULL  && Ifc_Match(((Abstract *)(x))->type.of, ifc)) ? (x) : Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from abstract mismatch", (i32)(x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
