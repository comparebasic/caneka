enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef status (*RblFunc)(struct roebling *rbl, Match *mt);

typedef struct roebling {
    Type type;
    MemCtx *m;
    int idx;
    Abstract *source; 
    /* current */
    struct {
        Span *values;
        Span *ko;
    } matches;
    /* overall */
    Range range;
    Span *parsers_do;
    Lookup *gotos;
    RblFunc dispatch;
} Roebling;

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source);
status Roebling_Run(Roebling *sexp);
int Roebling_GetMarkIdx(Roebling *rlb, int mark);
status Roebling_SetMark(Roebling *rlb, int mark);
status Roebling_Prepare(Roebling *rbl);
status Roebling_SetLookup(Roebling *rbl, Lookup *lk);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length);
