typedef struct templ {
    Type type;
    Iter content;
    Iter nested;
} Templ;

Templ *Templ_Make(MemCh *m);
Templ *Templ_ToS(Templ *templ, Stream *sm);
