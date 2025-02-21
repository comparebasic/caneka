enum strsnip_flags {
    STRSNIP_CONTENT = 1 << 8,
    STRSNIP_GAP = 1 << 9,
    STRSNIP_UNCLAIMED = 1 << 10,
};

typedef struct strsnip {
    Type type;
    int start;
    int length;
} StrSnip;

#define StrSnip_Total(sn) (sn->start+sn->length)
status StrSnip_Init(StrSnip *sn, word flags, int start, int length);
StrSnip *StrSnip_Add(MemCtx *m, String *s, int start, int length);
