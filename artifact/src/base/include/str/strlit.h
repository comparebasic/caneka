typedef struct strlit {
    RangeType type;
    /* bytes here */
} StrLit;


#define Bytes_Alloc(m, alloc) StrLit_BytesMake((m), (alloc))
#define BytesPage_Alloc(m, alloc) StrLit_PageBytesMake((m), (alloc))

StrLit *StrLit_Make(MemCh *m, i16 alloc);
byte *StrLit_BytesMake(MemCh *m, word alloc);
byte *StrLit_PageBytesMake(MemPage *pg, word sz);
