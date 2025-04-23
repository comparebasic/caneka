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

    void *args[] = {&rbl->matchIt.span->nvalues , NULL};
    r |= Test(rbl->matchIt.span->nvalues == 4, "Roebling has four match values loaded up, have _i4", args);

    MemCh_Free(m);
    return r;
}

status RoeblingRun_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    /*

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, NULL, String_Init(m, STRING_EXTEND), Found, NULL); 

    Str *s = NULL; 
    s = String_Make(m, bytes("TWO for the weekend\n"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_RunCycle(rbl);

    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT");
    Match *mt = Roebling_GetMatch(rbl);

    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals expected, have %s", s->bytes);
    i32 idx = Roebling_GetMatchIdx(rbl);
    r |= Test(idx = 1, "Match Idx equals expected");

    Roebling_RunCycle(rbl);
    mt = Roebling_GetMatch(rbl);
    s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);

    r |= Test(String_EqualsBytes(s, bytes("for the weekend")), "Roebling has captured the rest of the line, expected 'for the weekend', have '%s'", String_ToChars(m, s));
    r |= Test((rbl->type.state & ROEBLING_NEXT) != 0, "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    r |= Test((rbl->type.state & SUCCESS) != 0, "Roebling has state SUCCESS");
    */

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
