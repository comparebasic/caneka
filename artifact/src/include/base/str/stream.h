enum stream_flags {
    STREAM_STRVEC = 1 << 8,
    STREAM_CHAIN = 1 << 9,
    STREAM_FROM_FD = 1 << 10,
    STREAM_TO_FD = 1 << 11,
    STREAM_MESS = 1 << 12,
    STREAM_SOCKET = 1 << 13,
    STREAM_BUFFER = 1 << 14,
    STREAM_CREATE = 1 << 15,
};

typedef i64 (*StreamFunc)(struct stream *sm, byte *b, i32 length);
typedef i64 (*StreamAbsFunc)(struct stream *sm, Abstract *a);

typedef struct stream {
    Type type;
    i32 fd;
    i32 indent;
    MemCh *m;
    StreamFunc func;
    union {
        Cursor *curs;
        struct mess *mset;
        Iter *it;
    } dest;
    Abstract *source;
} Stream;

typedef struct stream_task {
    Type type;
    i32 flags32;
    Stream *sm;
    Abstract *a;
    StreamAbsFunc func;
} StreamTask;

i64 Stream_Bytes(Stream *sm, byte *b, i32 length);
i64 Stream_VecTo(Stream *sm, StrVec *v);
i64 Stream_Read(Stream *sm, i32 length);
status Stream_SetupMakeStrVec(MemCh *m, Stream *sm, StrVec *v);
Stream *Stream_MakeStrVec(MemCh *m);
Stream *Stream_MakeChain(MemCh *m, Span *chain);
Stream *Stream_MakeFromFd(MemCh *m, i32 fd, word flags);
Stream *Stream_MakeToFd(MemCh *m, i32 fd, StrVec *v, word flags);
Stream *Stream_Make(MemCh *m);
StreamTask *StreamTask_Make(MemCh *m, Stream *sm, Abstract *a, StreamAbsFunc func);
i64 Stream_IndentOut(Stream *sm);
