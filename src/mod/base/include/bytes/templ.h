typedef struct templ {
    Type type;
    i32 currentIdx;
    Span *p;
    void *args;
} Templ;

status Templ_Render(struct buff *bf, Templ *templ, struct arr *args);
StrVec *Templ_ToVec(MemCh *m, Templ *templ, struct arr *args);
Str *Templ_ToStr(MemCh *m, Templ *templ, struct arr *args);
Templ *Templ_FromCstr(MemCh *m, char *cstr);
Templ *Templ_From(MemCh *m, byte *b, i16 length);
