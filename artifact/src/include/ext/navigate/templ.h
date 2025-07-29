typedef struct templ {
    Type type;
    i32 indent;
    Iter it;
    Iter content;
    struct nested *data;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content);
i64 Stream_Templ(Stream *sm, Templ *templ, struct nested *data);
