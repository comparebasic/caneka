#define BUFF_CYCLE_MAX 512

enum send_recv_flags {
    BUFF_OUT = 1 << 8,
    BUFF_IN = 1 << 9,
    BUFF_ASYNC = 1 << 10,
    BUFF_FD = 1 << 11,
    BUFF_SOCKET = 1 << 11,
    BUFF_STRVEC = 1 << 13,
    BUFF_FLUSH = 1 << 14,
    BUFF_UNBUFFERED = 1 << 15,
};

typedef struct buff {
    Type type;
    i32 fd;
    MemCh *m;
    StrVec *v;
    struct {
        Str *s;
        i32 idx;
    } tail;
    struct {
        Str *s;
        i64 total;
        i32 idx;
    } unsent;
} Buff;

status Buff_AddBytes(Buff *bf, byte *bytes, word length);
Buff *Buff_Make(MemCh *m, word flags);
