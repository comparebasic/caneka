enum stream_flags {
    STREAM_STRVEC = 1 << 8,
    STREAM_CHAIN = 1 << 9,
    STREAM_FROM_FD = 1 << 10,
    STREAM_TO_FD = 1 << 11,
    STREAM_MESS = 1 << 12,
    STREAM_SOCKET = 1 << 13,
    STREAM_BUFFER = 1 << 14,
};

typedef i64 (*StreamFunc)(struct stream *sm, byte *b, i32 length);

typedef struct stream {
    Type type;
    i32 fd;
    MemCh *m;
    StreamFunc func;
    union {
        Cursor *curs;
        struct mess *mset;
        Iter *it;
    } dest;
    Abstract *source;
} Stream;

Stream *Stream_Make(MemCh *m);
i64 Stream_To(Stream *sm, byte *b, i32 length);
i64 Stream_VecTo(Stream *sm, StrVec *v);
status Stream_SetupMakeStrVec(MemCh *m, Stream *sm, StrVec *v);
Stream *Stream_MakeStrVec(MemCh *m);
Stream *Stream_MakeChain(MemCh *m, Span *chain);
Stream *Stream_MakeFromFd(MemCh *m, i32 fd, word flags);
Stream *Stream_MakeToFd(MemCh *m, i32 fd, StrVec *v, word flags);
