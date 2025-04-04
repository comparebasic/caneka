enum stream_flags {
    STREAM_128_BYTES = 1 << 8,
    STREAM_512_BYTES = 1 << 9,
};

typedef i64 (*StreamFunc)(struct stream *sm, byte *b, i32 length);

typedef struct stream {
    Type type;
    i32 fd;
    MemCh *m;
    StrVec *v;
    Abstract *source;
    StreamFunc func;
    Iter it;
} Stream;

Stream *Stream_Make(MemCh *m, i32 fd, word flags, StreamFunc func);
i64 Stream_To(Stream *sm, byte *b, i32 length);
