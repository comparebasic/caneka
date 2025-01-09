enum strsnip_flags {
    STRSNIP_TRUNCATED = 1 << 8,
    STRSNIP_GAP = 1 << 9,
    STRSNIP_ABSOLUTE = 1 << 10,
    STRSNIP_COPIED = 1 << 10,
};

typedef struct strsnip {
    Type type;
    int start;
    int length;
} StrSnip;

typedef struct strsnip {
    Type type;
    int offset;
    int local;
} StrSnipAbs;

#define StrSnip_Total(sn) (sn->start+sn->length)
status StrSnip_Init(StrSnip *sn, int start, int length);
