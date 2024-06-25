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
    ParserMaker *parsers;
} Roebling;

Roebling *Roebling_Make(MemCtx *m, cls type, ParserMaker *parsers, String *s, void *source);
status Roebling_Run(Roebling *sexp);
