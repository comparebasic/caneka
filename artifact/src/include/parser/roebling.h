#define ROEBLING_GUARD_MAX 100000

typedef status (*RblFunc)(MemCh *m, struct roebling *rbl);
typedef status (*RblCaptureFunc)(word captureKey, int matchIdx, Str *s, Abstract *source);

enum roebling_flags {
    /*MATCH_INVERTED = 1 << 9,*/
    ROEBLING_NEXT = 1 << 10,
    ROEBLING_REPEAT = 1 << 11,
    ROEBLING_LOAD_MATCHES = 1 << 12,
};

enum mark_reserved {
    LOOP_MARK = 0,
    _FIRST_MARK,
};

typedef struct roebling_blank {
    Type type;
    MemCh *m;
    Cursor cursor;
    Abstract *source; 
    RblCaptureFunc capture;
} RoeblingBlank;

typedef struct roebling {
    Type type;
    i32 idx;
    MemCh *m;
    Cursor *curs;
    Abstract *source; 
    RblCaptureFunc capture;
    Span *snips;
    Span *parsers_do;
    Lookup *marks;
    i8 jump;
    i8 jumpMiss;
    i32 tail;
    i32 length;
    Lookup *markLabels;
    Guard guard;
    Iter matchIt;
} Roebling;

/* > Instantation */
Roebling *Roebling_Make(MemCh *m,
    cls type,
    Lookup *markLabels,
    Cursor *curs,
    RblCaptureFunc capture,
    Abstract *source
);

status Roebling_AddParsers(MemCh *m, Span *parsers, Lookup *marks, Span *additions, Lookup *desc);

/* > Reset */
status Roebling_Reset(MemCh *m, Roebling *rbl, Str *s);

/* > Debug */
Str *Roebling_GetMarkDebug(Roebling *rbl, int idx);
Match *Roebling_GetMatch(Roebling *rbl);
int Roebling_GetMatchIdx(Roebling *rbl);
Match *Roebling_GetValueMatch(Roebling *rbl);

/* > Run */
status Roebling_Run(Roebling *sexp);
status Roebling_RunCycle(Roebling *sexp);
status Roebling_JumpTo(Roebling *rbl, int mark);

/* > Setup Cycle */
status Roebling_Add(Roebling *rbl, Str *s);
status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length);
status Roebling_AddReset(Roebling *rbl);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, int jump);
status Roebling_SetLookup(Roebling *rbl, Lookup *lk, word captureKey, int jump);
int Roebling_GetMarkIdx(Roebling *rlb, int mark);
Match *Roebling_LatestMatch(Roebling *rbl);
