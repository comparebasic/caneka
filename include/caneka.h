typedef unsigned char byte;
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
struct parser;
struct span;
struct span_slab;
struct structexp;
struct lookup;

typedef struct typehdr {
    cls of;
    status state;
} Type;

typedef struct virt  {
    Type type;
} Abstract;

typedef struct virt * AbstractPtr;

#define MAX_BASE10 20
#define SPAN_DIM_SIZE 16
#define SLAM_MIN_SIZE 4
#define SLAB_START_SIZE 2
#define SLAB_BYTE_SIZE (SPAN_DIM_SIZE*sizeof(Abstract *))
#define STRING_CHUNK_SIZE ((SLAB_BYTE_SIZE - (sizeof(struct typehdr)+sizeof(word)+sizeof(struct string *)))-1)
#define STRING_FIXED_SIZE (64  - (sizeof(struct typehdr)+sizeof(word)))-1

#define as(x, t) (((Abstract *)(x))->type.of == (t) ? x : Fatal("Cast from abstract mismatch", t))


#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define TRUE 1
#define FALSE 0


enum types {
    _TYPE_START,
    TYPE_ABSTRACT,
    TYPE_UNIT,
    TYPE_MEMCTX,
    TYPE_MAKER,
    TYPE_MEMSLAB,
    TYPE_REQ,
    TYPE_PROTO,
    TYPE_PROTODEF,
    TYPE_STRING_CHAIN,
    TYPE_STRING_FIXED,
    TYPE_SERVECTX,
    TYPE_TESTSUITE,
    TYPE_PARSER,
    TYPE_MULTIPARSER,
    TYPE_SCURSOR,
    TYPE_RANGE,
    TYPE_MATCH,
    TYPE_STRINGMATCH,
    TYPE_PATMATCH,
    TYPE_PATCHARDEF,
    TYPE_STRUCTEXP,
    TYPE_SPAN,
    TYPE_SLAB,
    TYPE_CHAIN,
    TYPE_HASHED,
    TYPE_HASHED_LINKED,
    TYPE_TABLE,
    TYPE_STRINGTABLE,
    TYPE_LOOKUP,
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
    CYCLE_MARK = 1 << 2,
    CYCLE_LOOP = 1 << 3,
    CYCLE_BREAK = 1 << 4,
    INCOMING = 1 << 5,
    PROCESSING = 1 << 6,
    RESPONDING = 1 << 7,
    SUCCESS = 1 << 8,
    RAW = 1 << 9,
    MISS = 1 << 10,
    HASHED = 1 << 11,
};

#define HasFlag(x, fl) ((x & (fl)) == fl) 

enum positions {
    POSITION_CONTAINS = 0,
    POSITION_START = 1,
    POSITION_END,
};

char *State_ToString(status state);
char *Class_ToString(cls type);

#define COMPLETE SUCCESS
#define TEST_OK READY

typedef struct parser *(*ParserMaker)(struct structexp *sexp); /* pmk */
#include "chain.h"
#include "error.h"
#include "log.h"
#include "mem.h"
#include "compare.h"
#include "hash.h"
#include "maker.h"
#include "string.h"
#include "slab.h"
#include "span.h"
#include "table.h"
#include "tokens.h"
#include "match.h"
#include "scursor.h"
#include "lookup.h"
#include "roebling.h"
#include "parsers.h"
#include "apps.h"
#include "proto.h"
#include "serve.h"
#include "req.h"
#include "debug.h"

status Caneka_Init(MemCtx *m);
