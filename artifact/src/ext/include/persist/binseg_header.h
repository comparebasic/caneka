typedef struct binseg_hdr {
    quad total;
    word kind;
    word id;
    /* data */
} BinSegHeader;

i16 BinSegHeader_FooterSize(quad total, word kind);
BinSegHeader *BinSegHeader_Make(BinSegCtx *ctx,
    quad total, word kind, word id, Str **content, Str **entry);
