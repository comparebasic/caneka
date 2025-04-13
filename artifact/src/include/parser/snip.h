enum strvec_snip {
    SNIP_CONTENT = 1 << 8,
    SNIP_GAP = 1 << 9,
    SNIP_STRVEC_END = 1 << 10,
    SNIP_UNCLAIMED = 1 << 11,
};

struct snip {
    Type type;
    i32 length;
} Snip;

StrVec *StrVec_Snip(MemCh *m, Span *sns, Cursor *curs);
