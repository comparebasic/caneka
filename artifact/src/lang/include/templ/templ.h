typedef struct templ {
    Type type;
    MemCh *m;
    i32 indent;
    i16 level;
    Iter content;
    Iter data;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content);
status Templ_Prepare(Templ *templ);
i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total, Abstract *source);
i64 Templ_ToS(Templ *templ, Stream *sm, Abstract *data, Abstract *source);
