i64 Stream_ToB64(Stream *sm, byte *b, i32 length);
i64 Stream_ToTea(Stream *sm, byte *b, i32 length);
i64 Stream_ToAes(Stream *sm, byte *b, i32 length);
i64 Stream_ToDigest(Stream *sm, byte *b, i32 length);
i64 Stream_ToSign(Stream *sm, byte *b, i32 length);
i64 Stream_ToVerify(Stream *sm, byte *b, i32 length);
i64 Stream_ToStrVec(Stream *sm, byte *b, i32 length);
