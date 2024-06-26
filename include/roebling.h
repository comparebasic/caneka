enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef lookup {
    word offset;
    Span *values;
    Virtual *arg;
} RblLookup;

typedef struct structexp {
    cls type;
    MemCtx *m;
    status state;
    void *source; 
    Span *marks;
    Range range;
    ParserMaker *parsers;
} Roebling;

typedef status (*RlbLookupPopulate)(MemCtx *m, RlbLookup *lk);

RlbLookup RlbLookup_Make(MemCtx *m, word offset, RlbLookupPopulate populate, Virtual *arg);
Roebling *Roebling_Make(MemCtx *m, cls type, ParserMaker *parsers, String *s, void *source);
status Roebling_Run(Roebling *sexp);
