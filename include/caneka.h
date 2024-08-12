typedef unsigned char byte;
typedef char i8;
typedef int16_t i16;
typedef uint16_t word;
typedef word cls;
typedef word status;
typedef byte boolean;
typedef uint32_t dword;
typedef uint64_t i64;
typedef i64 util;

struct serve_ctx;
struct serve_req;
struct mem_ctx;
struct mem_slab;
struct strcursor_range;
struct span;
struct span_slab;
struct structexp;
struct lookup;
struct strcursor;
struct strcursor_range;
struct roebling;

typedef struct typehdr {
    cls of;
    status state;
} Type;

typedef struct virt  {
    Type type;
} Abstract;

typedef struct virt * AbstractPtr;

#define MAX_BASE10 20
#define SLAB_START_SIZE 2


#define SLAB_BYTE_SIZE (16 * sizeof(void *))
#define STRING_CHUNK_SIZE ((SLAB_BYTE_SIZE - (sizeof(struct typehdr)+sizeof(word)+sizeof(struct string *)))-1)
#define STRING_FIXED_SIZE (64  - (sizeof(struct typehdr)+sizeof(word)))-1

#define as(x, t) (((Abstract *)(x))->type.of == (t) ? x : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfc(x, ifc) (Ifc_Match(((Abstract *)(x))->type.of, ifc) ? x : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define TRUE 1
#define FALSE 0


enum types {
    _TYPE_START,
    TYPE_UNKNOWN,
    TYPE_ABSTRACT,
    TYPE_WRAPPED,
    TYPE_WRAPPED_FUNC,
    TYPE_WRAPPED_DO,
    TYPE_WRAPPED_UTIL,
    TYPE_WRAPPED_PTR,
    TYPE_UTIL,
    TYPE_UNIT,
    TYPE_MEMCTX,
    TYPE_MESS,
    TYPE_MAKER,
    TYPE_MEMSLAB,
    TYPE_REQ,
    TYPE_PROTO,
    TYPE_PROTODEF,
    TYPE_STRING,
    TYPE_STRING_CHAIN,
    TYPE_STRING_FIXED,
    TYPE_SERVECTX,
    TYPE_TESTSUITE,
    TYPE_PARSER,
    TYPE_ROEBLING,
    TYPE_MULTIPARSER,
    TYPE_SCURSOR,
    TYPE_RANGE,
    TYPE_MATCH,
    TYPE_STRINGMATCH,
    TYPE_PATMATCH,
    TYPE_PATCHARDEF,
    TYPE_STRUCTEXP,
    TYPE_SPAN,
    TYPE_SPAN_PTR,
    TYPE_SPAN_INLINE,
    TYPE_QUEUE,
    TYPE_SLAB,
    TYPE_CHAIN,
    TYPE_HASHED,
    TYPE_HASHED_LINKED,
    TYPE_TABLE,
    TYPE_STRINGTABLE,
    TYPE_LOOKUP,
    TYPE_ITER,
    TYPE_SINGLE,
    TYPE_RBL_MARK,
    _TYPE_CORE_END,
};

extern int METHOD_UNKOWN;
extern int METHOD_GET;
extern int METHOD_SET;
extern int METHOD_UPDATE;
extern int METHOD_DELETE;

enum status_types {
    READY = 0,
    ERROR = 1 << 0,
    NOOP = 1 << 1,
    LOOP = 1 << 2,
    BREAK = 1 << 3,
    TRACKED = 1 << 4,
    INCOMING = 1 << 5,
    PROCESSING = 1 << 6,
    RESPONDING = 1 << 7,
    SUCCESS = 1 << 8,
    RAW = 1 << 9,
    MISS = 1 << 10,
    HASHED = 1 << 11,
    INVERTED = 1 << 12,
    END = 1 << 13,
    NEXT = 1 << 14,
    KO = 1 << 15,
};

#define HasFlag(x, fl) ((x & (fl)) == (fl)) 

enum positions {
    POSITION_CONTAINS = 0,
    POSITION_START = 1,
    POSITION_END,
};

char *State_ToString(status state);
char *Class_ToString(cls type);

#define COMPLETE SUCCESS
#define TEST_OK READY

typedef boolean (*EqualFunc)(Abstract *a, void *b); /* eq */
boolean Abs_Eq(Abstract *a, void *b);
boolean Ifc_Match(cls inst, cls ifc);

#include "chain.h"
#include "error.h"
#include "log.h"
#include "mem.h"
#include "single.h"
#include "maker.h"
#include "compare.h"
#include "hash.h"
#include "string_cnk.h"
#include "int.h"
#include "slab.h"
#include "array.h"
#include "span.h"
#include "iter.h"
#include "table.h"
#include "tokens.h"
#include "match.h"
#include "scursor.h"
#include "range.h"
#include "lookup.h"
#include "debug.h"
#include "roebling.h"
#include "proto.h"
#include "mess.h"
#include "xml.h"
#include "serve.h"
#include "req.h"
#include "testsuite.h"
#include "mark.h"
#include "headers.h"
#include "apps.h"

status Caneka_Init(MemCtx *m);
