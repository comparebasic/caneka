typedef struct templ {
    Type type;
    MemCh *m;
    i32 nextIdx;
    Span *p;
} Templ;

status Templ_Render(struct buff *bf, Templ *templ, void *args[]);
StrVec *Templ_ToVec(MemCh *m, Templ *templ, void *args[]);
Str *Templ_ToStr(MemCh *m, Templ *templ, void *args[]);
void Templ_Add(Templ *templ, Str *s);
Templ *Templ_AddBytes(Templ *templ, byte *b, i16 length);
Templ *Templ_FromCstr(MemCh *m, char *cstr);
Templ *Templ_Make(MemCh *m);
