typedef char i8;
typedef unsigned char byte;
typedef int16_t i16;
typedef uint16_t word;
typedef int32_t i32;

typedef uint32_t quad;
typedef int64_t i64;
typedef uint64_t util;

#define DIGEST_SIZE 32
#define SIG_FOOTER_SIZE 64
#define SIGNATURE_SIZE 96

typedef byte digest[DIGEST_SIZE];

typedef word cls;
typedef word status;
typedef byte boolean;
typedef byte flags8;
typedef quad flags32;

typedef i64 microTime;

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

#define ZERO 0
#define MAX_BASE10 23

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
