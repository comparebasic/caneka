typedef unsigned char byte;
typedef uint16_t word;
typedef word cls;
typedef byte boolean;
typedef uint64_t i64;
typedef i64 Unit;
typedef int status;
typedef int type;

struct serve_ctx;
struct serve_req;
struct mem_ctx;
struct mem_slab;
struct strcursor_range;
struct parser;
struct span;
struct span_slab;
struct structexp;

typedef uint64_t Unit;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define TRUE 1
#define FALSE 0

enum types {
    TYPE_MEMCTX,
    TYPE_MEMSLAB,
    TYPE_REQ,
    TYPE_STRING,
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
};

enum positions {
    POSITION_CONTAINS = 0,
    POSITION_START = 1,
    POSITION_END,
};

char *State_ToString(status state);
char *Class_ToString(cls type);

#define COMPLETE SUCCESS
#define TEST_OK READY

typedef struct parser *(*ParserMaker)(struct structexp *sexp);
#include "error.h"
#include "log.h"
#include "mem.h"
#include "array.h"
#include "string.h"
#include "slab.h"
#include "span.h"
#include "tokens.h"
#include "match.h"
#include "scursor.h"
#include "structexp.h"
#include "parsers.h"
#include "serve.h"
#include "req.h"
#include "debug.h"
