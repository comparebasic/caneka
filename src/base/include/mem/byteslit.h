typedef struct strlit {
    RangeType type;
    /* bytes here */
} BytesLit;

BytesLit *BytesLit_Make(MemCh *m, i16 alloc, cls typeOf);
byte *Bytes_AllocOnPage(MemPage *pg, word sz, cls typeOf);
byte *Bytes_Alloc(MemCh *m, word alloc, cls typeOf);
