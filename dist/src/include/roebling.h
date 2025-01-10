#define ROEBLING_GUARD_MAX 1024

typedef status (*RblFunc)(MemCtx *m, struct roebling *rbl);
typedef status (*RblCaptureFunc)(word captureKey, int matchIdx, String *s, Abstract *source);

enum roebling_flags {
    /*MATCH_INVERTED = 1 << 9,*/
    ROEBLING_NEXT = 1 << 10,
    ROEBLING_REPEAT = 1 << 11,
};

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
    Span *parsers_do;
    Lookup *marks;
    RblCaptureFunc capture;
    /* run specific */
    Cursor cursor;
    /* step specific */
    i8 jump;
    i8 jumpMiss;
    int tail;
    /* debug */
    Lookup *markLabels;
    Guard guard;
} Roebling;


/* > Instantation */
Roebling *Roebling_Make(MemCtx *m,
    cls type,
    Span *parsers,
    Lookup *markLabels,
    String *s,
    RblCaptureFunc capture,
    Abstract *source
);
status Roebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks, Span *additions, Lookup *desc);

/* > Reset */
status Roebling_Reset(MemCtx *m, Roebling *rbl, String *s);

/* > Debug */
String *Roebling_GetMarkDebug(Roebling *rbl, int idx);
Match *Roebling_GetMatch(Roebling *rbl);
int Roebling_GetMatchIdx(Roebling *rbl);
Match *Roebling_GetValueMatch(Roebling *rbl);

/* > Run */
status Roebling_Run(Roebling *sexp);
status Roebling_RunCycle(Roebling *sexp);
status Roebling_JumpTo(Roebling *rbl, int mark);

/* > Setup Cycle */
status Roebling_Add(Roebling *rbl, String *s);
status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, int jump);
status Roebling_SetLookup(Roebling *rbl, Lookup *lk, word captureKey, int jump);
int Roebling_GetMarkIdx(Roebling *rlb, int mark);
