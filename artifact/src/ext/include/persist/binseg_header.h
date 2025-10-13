typedef struct binseg_hdr {
    quad total;
    word kind;
    word id;
    /* data */
} BinSegHeader;

i16 BinSegHeader_Size(BinSegHeader *hdr);
BinSegHeader *BinSegHeader_Make(MemCh *m,
    quad total, word kind, word id, Str **content, Str **entry);
