enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef struct structexp {
    Type type;
    MemCtx *m;
    status state;
    int idx;
    int mark;
    void *source; 
    Span *marks;
    Range range;
    Span *parsers_pmk;
    Lookup *gotos;
} Roebling;

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source);
status Roebling_Run(Roebling *sexp);
