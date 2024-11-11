/*
Caneka 
main include

This defines the types and structs for the runtime-polymorphic type system, and
all of Caneka.

related: core/caneka.c
*/
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

struct string;
struct serve_ctx;
struct serve_req;
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
struct roebling;
struct span_def;
struct mem_handle;
struct mem_keyed;
struct oset;
struct oset_def;
struct ioctx;

extern word GLOBAL_flags;

typedef struct typehdr {
    cls of;
    status state;
} Type;

typedef struct virt  {
    Type type;
} Abstract;

typedef struct reserve  {
    Type type;
    i32 nunits;
} Reserve;

extern Abstract Reserved;

typedef struct virt * AbstractPtr;
typedef status (*DoFunc)(struct mem_handle *mh);

#define ZERO 0
#define MAX_BASE10 22
#define SLAB_START_SIZE 2

#define MAX_DIMS 16

#define STRING_SEG_FOOTPRINT 256
#define STRING_EXTRAS (sizeof(Type)+sizeof(int)+sizeof(struct string *))
#define STRING_CHUNK_SIZE (STRING_SEG_FOOTPRINT - STRING_EXTRAS)-1
#define STRING_FIXED_SIZE (64  - (sizeof(struct typehdr)+sizeof(int)))-1

#define as(x, t) ((x) != NULL && ((Abstract *)(x))->type.of == (t) ? x : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define asIfc(x, ifc) (Ifc_Match(((Abstract *)(x))->type.of, ifc) ? x : Fatal("Cast from abstract mismatch", (x != NULL ? ((Abstract *)x)->type.of : TYPE_UNKNOWN)))

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define TRUE 1
#define FALSE 0

enum types {
    _TYPE_START,
    TYPE_UNKNOWN,
    TYPE_ABSTRACT,
    TYPE_RESERVE,
    TYPE_WRAPPED,
    TYPE_WRAPPED_FUNC,
    TYPE_WRAPPED_DO,
    TYPE_WRAPPED_UTIL,
    TYPE_WRAPPED_I64,
    TYPE_WRAPPED_PTR,
    TYPE_UTIL,
    TYPE_UNIT,
    TYPE_MEMCTX,
    TYPE_MEMHANDLE,
    TYPE_MHABSTRACT,
    TYPE_MESS,
    TYPE_MAKER,
    TYPE_MEMSLAB,
    TYPE_STRING,
    TYPE_STRING_CHAIN,
    TYPE_STRING_FIXED,
    TYPE_STRING_SLAB,
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
    TYPE_IOCTX,
    TYPE_ACCESS,
    _TYPE_SPAN_START,
    TYPE_SPAN,
    TYPE_MINI_SPAN,
    TYPE_MEM_SPAN,
    TYPE_QUEUE_SPAN,
    TYPE_QUEUE_IDX,
    TYPE_SLAB_SPAN,
    TYPE_STRING_SPAN,
    TYPE_TABLE,
    TYPE_POLL_MAP_SPAN,
    _TYPE_SPAN_END,
    TYPE_COORDS,
    TYPE_MEM_KEYED,
    TYPE_SPAN_STATE,
    TYPE_SPAN_DEF,
    TYPE_QUEUE,
    TYPE_CHAIN,
    TYPE_SPAN_QUERY,
    TYPE_SLAB,
    TYPE_TABLE_CHAIN,
    TYPE_HASHED,
    TYPE_HASHED_LINKED,
    TYPE_MEMPAIR,
    TYPE_STRINGTABLE,
    TYPE_FILE,
    TYPE_LOOKUP,
    TYPE_ITER,
    TYPE_SINGLE,
    TYPE_RBL_MARK,
    TYPE_OSET,
    TYPE_OSET_DEF,
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
    SUCCESS = 1 << 1,
    NOOP = 1 << 2,
    RAW = 1 << 3,
    HASHED = 1 << 4,
    MISS = 1 << 5,
    END = 1 << 6,
    TRACKED = 1 << 7,
    /* class speciric */
    LOCAL_PTR = 1 << 8,
    INVERTED = 1 << 9,
    INLINE = 1 << 10,
    BREAK = 1 << 11,
    NEXT = 1 << 12,
    INCOMING = 1 << 13,
    PROCESSING = 1 << 14,
    RESPONDING = 1 << 15,
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
#define ELASTIC INLINE
#define TERM_FOUND TRACKED
#define NO_COLOR RAW
#define MATCH_GOTO NEXT 

typedef boolean (*EqualFunc)(Abstract *a, void *b); /* eq */
boolean Abs_Eq(Abstract *a, void *b);
boolean Ifc_Match(cls inst, cls ifc);
cls Ifc_Get(cls inst);

#include "abstract.h"
#include "time_cnk.h"
#include "chain.h"
#include "error.h"
#include "log.h"
#include "mem.h"
#include "single.h"
#include "maker.h"
#include "compare.h"
#include "mempair.h"
#include "hash.h"
#include "string_cnk.h"
#include "int.h"
#include "slab.h"
#include "array.h"
#include "spandef.h"
#include "span.h"
#include "spanquery.h"
#include "iter.h"
#include "table.h"
#include "table_chain.h"
#include "tokens.h"
#include "match.h"
#include "scursor.h"
#include "range.h"
#include "lookup.h"
#include "debug.h"
#include "roebling.h"
#include "queue.h"
#include "handler.h"
#include "proto.h"
#include "mess.h"
#include "xml.h"
#include "oset.h"
#include "ioctx.h"
#include "memlocal.h"
#include "file.h"
#include "session.h"
#include "serve.h"
#include "req.h"
#include "testsuite.h"
#include "mark.h"
#include "apps.h"
#ifdef LINUX
    #include "linux.h"
#endif

status Caneka_Init(MemCtx *m);
