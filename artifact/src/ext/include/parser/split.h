enum split_flags {
    SPLIT_SKIP_FIRST_CHAR = 1 << 8,
};

status StrVec_Split(StrVec *v, Abstract *split);
Span *StrVec_ToSpan(MemCh *m, StrVec *v);
Span *StrVec_SplitToSpan(MemCh *m, StrVec *v, Abstract *split);
Span *Str_SplitToSpan(MemCh *m, Str *s, Abstract *split, word flags);
