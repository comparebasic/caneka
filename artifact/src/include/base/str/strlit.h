typedef struct strlit {
    RangeType type;
    /* bytes here */
} StrLit;

StrLit *StrLit_Make(MemCh *m, i16 alloc);
byte *StrLit_BytesMake(MemCh *m, i16 alloc);
