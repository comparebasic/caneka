enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef struct structexp {
    cls type;
    MemCtx *m;
    status state;
    void *source; 
    Span *marks;
    Range range;
    Span *parsers;
    Lookup *gotos;
} Roebling;

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Virtual *source);
status Roebling_Run(Roebling *sexp);
