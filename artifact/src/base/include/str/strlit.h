typedef struct strlit {
    RangeType type;
    /* bytes here */
} StrLit;


#ifdef INSECURE
    #define Bytes_Alloc(m, alloc) StrLit_BytesMake((m), (alloc))
    #define BytesPage_Alloc(m, alloc) StrLit_PageBytesMake((m), (alloc))
#else
    #define BytesPage_Alloc(m, alloc) MemPage_Alloc((m), (alloc))
    #define Bytes_Alloc(m, alloc) MemCh_Alloc((m), (alloc))
#endif

StrLit *StrLit_Make(MemCh *m, i16 alloc);
byte *StrLit_BytesMake(MemCh *m, word alloc);
byte *StrLit_PageBytesMake(MemPage *pg, word sz);
