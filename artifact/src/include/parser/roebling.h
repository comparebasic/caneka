#define ROEBLING_GUARD_MAX 100000

typedef status (*RblFunc)(MemCh *m, struct roebling *rbl);
typedef status (*RblCaptureFunc)(struct roebling *rbl, word captureKey, StrVec *v);

enum roebling_flags {
    ROEBLING_NEXT = 1 << 10,
    ROEBLING_REPEAT = 1 << 11,
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
    i32 _;
    MemCh *m;
    Cursor *curs;
    Abstract *source; 
    RblCaptureFunc capture;
    Span *snips;
    Iter parseIt;
    Lookup *marks;
    i32 tail;
    i32 length;
    Lookup *markLabels;
    Guard guard;
    Iter matchIt;
} Roebling;

status Roebling_RunCycle(Roebling *rbl);
status Roebling_JumpTo(Roebling *rbl, i32 mark);
status Roebling_Run(Roebling *rbl);
Match *Roebling_GetMatch(Roebling *rbl);
i32 Roebling_GetMatchIdx(Roebling *rbl);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, i32 jump);
i64 Roebling_GetMarkIdx(Roebling *rbl, i32 mark);
status Roebling_Reset(MemCh *m, Roebling *rbl, StrVec *v);
status Roebling_AddStep(Roebling *rbl, Abstract *step);
