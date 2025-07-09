typedef struct templ {
    Type type;
    Iter content;
    Iter data;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content, OrdTable *data);
i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total);
i64 Templ_ToS(Templ *templ, Stream *sm);
