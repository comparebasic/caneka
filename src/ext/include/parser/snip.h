enum strvec_snip {
    SNIP_CONTENT = 1 << 8, /* C */
    SNIP_GAP = 1 << 9, /* G */
    SNIP_STR_BOUNDRY = 1 << 10, /* B */
    SNIP_UNCLAIMED = 1 << 11, /* U */
    SNIP_SKIPPED = 1 << 12, /* K */
    SNIP_SEP = 1 << 13, /* S */
    SNIP_NOTAIL = 1 << 14, /*T */
};

typedef struct snip {
    Type type;
    i32 length;
} Snip;

StrVec *StrVec_Snip(MemCh *m, Span *sns, Cursor *curs);
Snip *Snip_From(MemCh *m, Snip *sn);
Snip *Snip_Make(MemCh *m);
status SnipSpan_Add(Span *sns, Snip *sn);
status SnipSpan_AddFrom(Span *sns, i32 length, word flags);
status SnipSpan_Remove(Span *sns, i32 length);
i64 SnipSpan_Total(Span *sns, word flags);
status SnipSpan_Set(MemCh *m, Span *sns, i32 length, word flags);
status SnipSpan_SetAll(Span *sns, word flags);
