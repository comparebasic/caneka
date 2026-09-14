typedef struct templ {
    Type type;
    Span *p;
    void *args;
} Templ;

Templ *Templ_From(MemCh *m, byte *b, i16 length);
Templ *Templ_FromCstr(MemCh *m, char *cstr);
