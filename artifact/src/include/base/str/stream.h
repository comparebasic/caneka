i64 Stream_To(Stream *sm, byte *b, i32 length);
i64 Stream_VecTo(Stream *sm, StrVec *v);
i64 Stream_Read(Stream *sm, i32 length);
status Stream_SetupMakeStrVec(MemCh *m, Stream *sm, StrVec *v);
Stream *Stream_MakeStrVec(MemCh *m);
Stream *Stream_MakeChain(MemCh *m, Span *chain);
Stream *Stream_MakeFromFd(MemCh *m, i32 fd, word flags);
Stream *Stream_MakeToFd(MemCh *m, i32 fd, StrVec *v, word flags);
Stream *Stream_Make(MemCh *m);
