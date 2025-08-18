i64 Stream_ToB64(Stream *sm, byte *b, i32 length);
i64 Stream_FromB64(Stream *sm, byte *b, i32 length);
StrVec *StrVec_FromBytes(MemCh *m, byte *b, i32 length);
i64 Stream_ToFd(Stream *sm, byte *b, i32 length);
i64 Stream_ToStrVec(Stream *sm, byte *b, i32 length);
