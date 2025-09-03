typedef struct templ {
    Type type;
    i32 indent;
    i16 level;
    Iter content;
    Iter data;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content);
i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total, Abstract *source);
i64 Templ_ToS(Templ *templ, Stream *sm, Abstract *data, Abstract *source);
