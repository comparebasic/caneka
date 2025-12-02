#include <external.h>
#include <caneka.h>
#include <test_module.h>
#include "tests.h"

enum rbl_test_enum {
    _start = 333,
    ONE,
    TWO,
    THREE,
};

enum rbl_test_marks {
    RBL_TEST_START = _TYPE_CORE_END,
    RBL_TEST_END,
};

static PatCharDef text[] = {
    {PAT_INVERT_CAPTURE|PAT_ANY|PAT_TERM, ' ', ' '},
    {PAT_KO, '\n', '\n'},
    {PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0}
};
static PatCharDef nl[] = {NL_DEF};
static PatCharDef dbl_nl[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static StrVec *lastResult = NULL;
static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    lastResult = v;
    return SUCCESS;
}

static PatCharDef oneDef[] = {
    {PAT_TERM, 'O', 'O'},
    {PAT_TERM, 'N', 'N'},
    {PAT_TERM, 'E', 'E'},
    {PAT_END, 0, 0},
};
static PatCharDef twoDef[] = {
    {PAT_TERM, 'T', 'T'},
    {PAT_TERM, 'W', 'W'},
    {PAT_TERM, 'O', 'O'},
    {PAT_END, 0, 0},
};
static PatCharDef threeDef[] = {
    {PAT_TERM, 'T', 'T'},
    {PAT_TERM, 'H', 'H'},
    {PAT_TERM, 'R', 'R'},
    {PAT_TERM, 'E', 'E'},
    {PAT_TERM, 'E', 'E'},
    {PAT_END, 0, 0},
};

status SetWord1(MemCh *m, void *a){
    status r = READY;
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, dbl_nl, 0, RBL_TEST_END);
    r |= Roebling_SetPattern(rbl, oneDef, 0, -1);
    r |= Roebling_SetPattern(rbl, twoDef, 0, -1);
    r |= Roebling_SetPattern(rbl, threeDef, 0, -1);
 
    return r; 
}

status SetWord2(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, text, 0, RBL_TEST_START);
    return r; 
}

status Roebling_Tests(MemCh *m){
    status r = READY;
    Str *s = NULL; 
    StrVec *v = StrVec_Make(m);
    Cursor *curs = Cursor_Make(m, v);

    Roebling *rbl = NULL;
    rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord1));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord2));
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_END));
    Roebling_Start(rbl);
    rbl->type.state |= ROEBLING_REPEAT;

    Single *dof = (Single *)as(Span_Get(rbl->parseIt.p, 0), TYPE_WRAPPED_DO);
    ((RblFunc)dof->val.dof)(rbl->m, rbl);

    void *args[] = {
        I32_Wrapped(m, rbl->matchIt.p->nvalues),
        NULL
    };
    r |= Test(rbl->matchIt.p->nvalues == 4, "Roebling has four match values loaded up, have $", args);

    return r;
}

status RoeblingRun_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;

    Str *s = NULL; 
    StrVec *v = NULL;
    Roebling *rbl = NULL;

    Cursor *curs = Cursor_Make(m, StrVec_Make(m));
    rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord1));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord2));
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_END));
    Roebling_Start(rbl);

    s = Str_CstrRef(m, "TWO for the weekend\n");
    Cursor_Add(curs, s);
    Roebling_RunCycle(rbl);

    void *args[] = {
        rbl,
        NULL
    };
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING-NEXT", NULL);
    Match *mt = Roebling_GetMatch(rbl);

    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    s = Str_CstrRef(m, "TWO");
    void *args1[] = {
        s,
        v,
        NULL
    };
    r |= Test(Equals(v, s), "Content equals expected '@', have @", args1);
    i32 idx = Roebling_GetMatchIdx(rbl);
    void *args2[] = {
        I32_Wrapped(m, idx),
        NULL
    };
    r |= Test(idx = 1, "Match Idx equals expected, have $", args2);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);

    s = Str_CstrRef(m, "for the weekend");
    void *args3[] = {
        s,
        v,
        NULL
    };
    r |= Test(Equals(v, s),
        "Roebling has captured the rest of the line, expected '$', have '@'", args3);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING-NEXT", NULL);

    DebugStack_Pop();
    return r;
}

status RoeblingMark_Tests(MemCh *m){
    status r = READY;
    Str *s = NULL;
    StrVec *v = NULL;
    Match *mt = NULL;
    Roebling *rbl = NULL;

    Cursor *curs = Cursor_Make(m, StrVec_Make(m));
    rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord1));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord2));
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_END));
    Roebling_Start(rbl);

    s = Str_CstrRef(m, "TWO for the weekend\nONE for good measure\nTHREE for all!\n\n");
    Cursor_Add(curs, s);

    void *args[] = {
        s,
        NULL
    };

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    void *args1[] = {
        v,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "TWO")), "Content equals TWO, have @", args1);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT after 'TWO'", NULL);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    void *args2[] = {
        v,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "for the weekend")), "Roebling has captured the rest of the line: @", args2);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT after 'for the weekend'", NULL);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    void *args3[] = {
        v,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "ONE")), "Content equals ONE, have @", args3);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT after 'ONE'", NULL);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    void *args4[] = {
        v,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "for good measure")), "Roebling has captured the rest of the line: @", args4);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT", NULL);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    void *args5[] = {
        v,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "THREE")), "Content equals THREE, have @", args5);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT", NULL);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    void *args6[] = {
        v,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "for all!")), "Roebling has captured the rest of the line: @", args6);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT", NULL);

    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    void *args7[] = {
        State_ToStr(m, rbl->type.state),
        NULL
    };
    r |= Test((rbl->type.state & SUCCESS) != 0, "Roebling has state SUCCESS, have $", args7);

    return r;
}

status RoeblingStartStop_Tests(MemCh *m){
    status r = READY;
    Str *s = NULL;
    StrVec *v = NULL;
    Match *mt = NULL;
    Roebling *rbl = NULL;

    Cursor *curs = Cursor_Make(m, StrVec_Make(m));
    rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)SetWord2));
    Roebling_AddStep(rbl, I16_Wrapped(m, RBL_TEST_END));
    Roebling_Start(rbl);

    s = Str_CstrRef(m, "Hi how are you ");
    Cursor_Add(curs, s);
    Roebling_Run(rbl);

    s = Str_CstrRef(m, "today?\n");
    Cursor_Add(curs, s);
    Roebling_Run(rbl);

    mt = Roebling_GetMatch(rbl);
    void *args1[] = {
        mt,
        rbl,
        NULL
    };
    v = lastResult;
    v->type.state |= DEBUG;
    void *args[] = {
        v,
        rbl,
        NULL
    };
    r |= Test(Equals(v, Str_CstrRef(m, "Hi how are you today?")), "String equals 'Hi how are you today?', have '@' for @", args);

    return r;
}
