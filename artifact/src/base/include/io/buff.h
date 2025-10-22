enum send_recv_flags {
    SEND_RECV_ASYNC = 1 << 8,
    SEND_RECV_FD = 1 << 9,
    SEND_RECV_SOCKET = 1 << 10,
    SEND_RECV_STRVEC = 1 << 11,
    SEND_RECV_FLUSH = 1 << 12,
};

typedef struct send {
    Type type;
    i32 fd;
    StrVec *buff;
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

status Buff_AddBytes(SendRecv *sr, byte *bytes, word length);
