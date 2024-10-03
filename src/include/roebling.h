typedef status (*RblFunc)(struct roebling *rbl);
typedef status (*RblCaptureFunc)(word captureKey, String *s, Abstract *source);

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
    Span * matches;
    /* overall */
    Range range;
    Span *parsers_do;
    Lookup *marks;
    /* step specific */
    RblCaptureFunc capture;
    i8 jump;
    i8 jumpMiss;
    int tail;
    /* debug */
    Lookup *markLabels;
} Roebling;

Roebling *Roebling_Make(MemCtx *m,
    cls type,
    Span *parsers,
    Lookup *markLabels,
    String *s,
    RblCaptureFunc capture,
    Abstract *source
);
status Roebling_Run(Roebling *sexp);
int Roebling_GetMarkIdx(Roebling *rlb, int mark);
status Roebling_SetMark(Roebling *rlb, int mark, int idx);
status Roebling_Prepare(Roebling *rbl, Span *parsers);
status Roebling_SetLookup(Roebling *rbl, Lookup *lk, word captureKey, int jump);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, int jump);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length);
int Roebling_GetMatchIdx(Roebling *rbl);
Match *Roebling_GetMatch(Roebling *rbl);
Match *Roebling_GetValueMatch(Roebling *rbl);
status SCursor_Finish(Roebling *rbl, Match *mt);
/* debug */
String *Roebling_GetMarkDebug(Roebling *rbl, int idx);
