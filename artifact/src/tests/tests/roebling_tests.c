#include <external.h>
#include <caneka.h>

enum rbl_test_enum {
    _start = 333,
    ONE,
    TWO,
    THREE,
};

enum rbl_test_marks {
    RBL_TEST_START = 1000,
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

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
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

status SetWord1(MemCh *m, Abstract *a){
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

status Roebling_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 
    StrVec *v = StrVec_Make(m);
    Cursor *curs = Cursor_Make(m, v);

    Roebling *rbl = NULL;
    rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2));
    rbl->type.state |= ROEBLING_REPEAT;

    Single *dof = (Single *)as(Span_Get(rbl->parseIt.span, 0), TYPE_WRAPPED_DO);
    ((RblFunc)dof->val.dof)(rbl->m, rbl);

    Abstract *args[] = {
        (Abstract *)I32_Wrapped(m, rbl->matchIt.span->nvalues),
        NULL
    };
    r |= Test(rbl->matchIt.span->nvalues == 4, "Roebling has four match values loaded up, have $", args);

    MemCh_Free(m);
    return r;
}

status RoeblingRun_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *s = NULL; 
    StrVec *v = NULL;
    Cursor *curs = Cursor_Make(m, StrVec_Make(m));
    Roebling *rbl = NULL;

    rbl = Roebling_Make(m, curs, Capture, NULL); 
    rbl->type.state |= DEBUG;
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2));
    Roebling_Start(rbl);

    s = Str_CstrRef(m, "TWO for the weekend\n");
    Cursor_Add(curs, s);
    Roebling_RunCycle(rbl);

    Abstract *args[] = {
        (Abstract *)rbl,
        NULL
    };
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING-NEXT", NULL);
    Match *mt = Roebling_GetMatch(rbl);

    v = StrVec_Snip(rbl->m, mt->backlog, curs);
    s = Str_CstrRef(m, "TWO");
    Abstract *args1[] = {
        (Abstract *)s,
        (Abstract *)v,
        NULL
    };
    r |= Test(Equals((Abstract *)v, (Abstract *)s), "Content equals expected '@', have @", args1);
    i32 idx = Roebling_GetMatchIdx(rbl);
    Abstract *args2[] = {
        (Abstract *)I32_Wrapped(m, idx),
        NULL
    };
    r |= Test(idx = 1, "Match Idx equals expected, have $", args2);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    v = StrVec_Snip(rbl->m, mt->backlog, curs);

    s = Str_CstrRef(m, "for the weekend");
    Abstract *args3[] = {
        (Abstract *)s,
        (Abstract *)v,
        NULL
    };
    r |= Test(Equals((Abstract *)v, (Abstract *)s),
        "Roebling has captured the rest of the line, expected '$', have '@'", args3);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING-NEXT", NULL);

    Roebling_RunCycle(rbl);
    r |= Test((rbl->type.state & SUCCESS) != 0, "Roebling has state SUCCESS", NULL);

    DebugStack_Pop();
    MemCh_Free(m);
    return r;
}

status RoeblingMark_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Match *mt = NULL;
    /*

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m);
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_START)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_END)); 

    LookupConfig config[] = {
        {RBL_TEST_START, (Abstract *)String_Make(m, bytes("START"))},
        {RBL_TEST_END, (Abstract *)String_Make(m, bytes("END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);
    Str *_s = String_Init(m, STRING_EXTEND);
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, _s, Found, NULL); 

    Str *s = String_Make(m, bytes("TWO for the weekend\nONE for good measure\nTHREE for all!\n\n"));
    Roebling_AddBytes(rbl, s->bytes, s->length);

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals TWO, have %s", s->bytes);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT after 'TWO'");

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("for the weekend")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT after 'for the weekend'");

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);

    r |= Test(String_EqualsBytes(s, bytes("ONE")), "Content equals ONE, have %s", s->bytes);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT after 'ONE'");

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("for good measure")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("THREE")), "Content equals THREE, have %s", s->bytes);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("for all!")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    r |= Test((rbl->type.state & SUCCESS) != 0, "Roebling has state SUCCESS %s", State_ToChars(rbl->type.state));
    */

    MemCh_Free(m);
    return r;
}

status RoeblingStartStop_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Match *mt = NULL;
    /*

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 

    Str *s = String_Init(m, STRING_EXTEND);
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, NULL, s, Found, NULL); 

    char *cstr = "Hi how are you ";
    Roebling_AddBytes(rbl, bytes(cstr), strlen(cstr));
    Roebling_Run(rbl);

    cstr = "today?\n";
    Roebling_AddBytes(rbl, bytes(cstr), strlen(cstr));
    Roebling_Run(rbl);

    mt = Roebling_GetMatch(rbl);

    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("Hi how are you today?")), "String equals 'Hi how are you today?', have '%s'", (char *)s->bytes);
    */

    MemCh_Free(m);
    return r;
}
