enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

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
    Lookup *marks;
    /* step specific */
    DoFunc dispatch;
    i8 jump;
    i8 jumpMiss;
} Roebling;

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, int startMark, String *s, Abstract *source);
status Roebling_Run(Roebling *sexp);
int Roebling_GetMarkIdx(Roebling *rlb, int mark);
status Roebling_SetMark(Roebling *rlb, int mark, int idx);
status Roebling_Prepare(Roebling *rbl, Span *parsers);
status Roebling_SetLookup(Roebling *rbl, Lookup *lk);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def);
status Roebling_SetKOPattern(Roebling *rbl, PatCharDef *def);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length);
int Roebling_GetMatchIdx(Roebling *rbl);
Match *Roebling_GetMatch(Roebling *rbl);
Match *Roebling_GetValueMatch(Roebling *rbl);
status SCursor_Finish(Roebling *rbl, Match *mt);
