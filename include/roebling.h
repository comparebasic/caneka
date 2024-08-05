enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef struct structexp {
    Type type;
    MemCtx *m;
    int idx;
    Abstract *source; 
    /* current */
    struct parser parser;
    struct {
        Span *values;
        Span *ko;
    } matches;
    /* overall */
    Range range;
    Span *parsers_pmk;
    Lookup *gotos;
} Roebling;

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source);
status Roebling_Run(Roebling *sexp);
int Roebling_GetMarkIdx(Roebling *rlb, int mark);
status Roebling_SetMark(Roebling *rlb, int mark);
status Roebling_Prepare(Roebling *rbl);
status Roebling_SetLookup(Roebling *rbl, Lookup *lk);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def);
