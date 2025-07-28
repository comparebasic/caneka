typedef struct templ {
    Type type;
    i32 indent;
    Iter content;
    Nested *data;
    KeyFunc func;
    Abstract *source;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content);
i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total);
i64 Templ_ToS(Templ *templ, Stream *sm, Nested *data);
