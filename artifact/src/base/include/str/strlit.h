typedef struct strlit {
    RangeType type;
    /* bytes here */
} StrLit;

StrLit *StrLit_Make(MemCh *m, i16 alloc);
byte *Bytes_AllocOnPage(MemPage *pg, word sz);
byte *Bytes_Alloc(MemCh *m, word alloc);
