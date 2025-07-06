
#define RBL_GUARD_MAX GUARD_MAX

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

typedef struct roebling {
    Type type;
    i16 _;
    i16 guard;
    MemCh *m;
    Cursor *curs;
    Abstract *source; 
    struct mess *mess;
    RblCaptureFunc capture;
    Iter parseIt;
    Lookup *marks;
    i32 tail;
    i32 length;
    Lookup *markLabels;
    Iter matchIt;
} Roebling;

Roebling *Roebling_Make(MemCh *m, Cursor *curs, RblCaptureFunc capture, Abstract *source);
status Roebling_Start(Roebling *rbl);
status Roebling_RunCycle(Roebling *rbl);
status Roebling_JumpTo(Roebling *rbl, i32 mark);
status Roebling_Run(Roebling *rbl);
Match *Roebling_GetMatch(Roebling *rbl);
i32 Roebling_GetMatchIdx(Roebling *rbl);
status Roebling_ResetPatterns(Roebling *rbl);
status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, i16 jump);
i64 Roebling_GetMarkIdx(Roebling *rbl, i32 mark);
status Roebling_Reset(MemCh *m, Roebling *rbl, StrVec *v);
status Roebling_AddStep(Roebling *rbl, Abstract *step);
status Roebling_Finalize(Roebling *rbl, Match *mt, i64 total);
status Roebling_Dispatch(Roebling *rbl, Match *mt);
