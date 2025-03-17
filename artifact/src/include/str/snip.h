enum strsnip_flags {
    STRSNIP_CONTENT = 1 << 8,
    STRSNIP_GAP = 1 << 9,
    STRSNIP_UNCLAIMED = 1 << 10,
    STRSNIP_NEW_STR = 1 << 11,
};

typedef struct strsnip {
    Type type;
    32 length;
} StrSnip;

status StrSnip_Init(StrSnip *sn, word flags, int start, int length);
StrSnip *StrSnip_Add(MemCtx *m, String *s, int start, int length);
StrVec *StrVec_Snip(MemCtx *m, Span *sns, Cursor *curs);
