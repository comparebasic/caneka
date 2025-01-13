typedef struct strsnip {
    Type type;
    int start;
    int length;
} StrSnip;

#define StrSnip_Total(sn) (sn->start+sn->length)
status StrSnip_Init(StrSnip *sn, word flags, int start, int length);
StrSnip *StrSnip_Add(MemCtx *m, String *s, int start, int length);
