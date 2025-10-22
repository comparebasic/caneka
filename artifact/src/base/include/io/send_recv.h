enum send_recv_flags {
    SEND_RECV_ASYNC = 1 << 8,
    SEND_RECV_FD = 1 << 9,
    SEND_RECV_SOCKET = 1 << 10,
    SEND_RECV_STRVEC = 1 << 11,
};

typedef struct send {
    Type type;
    i32 fd;
    StrVec *buff;
    struct {
        Str *tail;
        i16 remaining;
    } tail;
    struct {
        Str *s;
        i64 total;
        i32 idx;
    } unsent;
} SendRecv;

status SendRecv_AddBytes(SendRecv *sr, byte *bytes, word length);
