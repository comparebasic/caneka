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

static status Found(word captureKey, int matchIdx, String *s, Abstract *source){
    if(DEBUG_ROEBLING){
        Debug_Print((void *)s, 0, "found s: ", COLOR_YELLOW, TRUE);
        printf("\n");
    }
    return SUCCESS;
}

static word oneDef[] = {
    PAT_TERM, 'O', 'O',
    PAT_TERM, 'N', 'N',
    PAT_TERM, 'E', 'E',
    PAT_END, 0, 0,
};
static word twoDef[] = {
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'W', 'W',
    PAT_TERM, 'O', 'O',
    PAT_END, 0, 0,
};
static word threeDef[] = {
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'H', 'H',
    PAT_TERM, 'R', 'R',
    PAT_TERM, 'E', 'E',
    PAT_TERM, 'E', 'E',
    PAT_END, 0, 0,
};

status SetWord1(MemCtx *m, Abstract *a){
    status r = READY;
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, (PatCharDef *)oneDef, 0, -1);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)twoDef, 0, -1);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)threeDef, 0, -1);
 
    return r; 
}

static word text[] = {PAT_INVERT_CAPTURE|PAT_ANY|PAT_TERM, ' ', ' ', PAT_KO, '\n', '\n', PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31, PAT_END, 0, 0};
static word nl[] = {NL_DEF};
static word nl_upper[] = {PAT_INVERT|PAT_TERM|PAT_INVERT_CAPTURE, '\n', '\n', PAT_END, 0, 0};
static word dbl_nl[] = {PAT_TERM, '\n', '\n', PAT_END, 0, 0};

status SetWord2(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)text, 0, -1);
    return r; 
}

status SetNextOrEnd(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)nl_upper, 0, RBL_TEST_START);
    r |= Roebling_SetPattern(rbl,(PatCharDef *)dbl_nl, 0, RBL_TEST_END);
    return r; 
}

status Roebling_Tests(MemCtx *gm){

    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    String *s = NULL; 

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, NULL, String_Init(m, STRING_EXTEND), Found, NULL); 

    Single *dof = as(Span_Get(rbl->parsers_do, 0), TYPE_WRAPPED_DO);
    ((RblFunc)dof->val.dof)(rbl->m, rbl);

    r |= Test(rbl->matches->nvalues == 3, "Roebling has three match values loaded up");

    MemCtx_Free(m);
    return r;
}

status RoeblingRun_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, NULL, String_Init(m, STRING_EXTEND), Found, NULL); 

    String *s = NULL; 
    s = String_Make(m, bytes("TWO for the weekend\n"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_RunCycle(rbl);

    Match *mt = Roebling_GetMatch(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    int idx = Roebling_GetMatchIdx(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals expected, have %s", s->bytes);
    r |= Test(idx = 1, "Match Idx equals expected");
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("for the weekend")), "Roebling has captured the rest of the line, expected 'for the weekend', have '%s'", s->bytes);
    /*
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    r |= Test(HasFlag(rbl->type.state, SUCCESS), "Roebling has state SUCCESS");
    */

    MemCtx_Free(m);
    return r;
}

status RoeblingMark_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_START)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetNextOrEnd)); 
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_END)); 

    LookupConfig config[] = {
        {RBL_TEST_START, (Abstract *)String_Make(m, bytes("START"))},
        {RBL_TEST_END, (Abstract *)String_Make(m, bytes("END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);
    String *_s = String_Init(m, STRING_EXTEND);
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, _s, Found, NULL); 

    String *s = String_Make(m, bytes("TWO for the weekend\nONE for good measure\nTHREE for all!\n\n"));
    Roebling_AddBytes(rbl, s->bytes, s->length);

    Roebling_RunCycle(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals TWO, have %s", s->bytes);
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT after 'TWO'");

    Roebling_RunCycle(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("for the weekend")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT after 'for the weekend'");

    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    s = Range_Copy(m, &(rbl->range));

    r |= Test(String_EqualsBytes(s, bytes("ONE")), "Content equals ONE, have %s", s->bytes);
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT after 'ONE'");

    Roebling_RunCycle(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("for good measure")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("THREE")), "Content equals THREE, have %s", s->bytes);
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("for all!")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test(HasFlag(rbl->type.state, ROEBLING_NEXT), "Roebling has state ROEBLING_NEXT");

    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    r |= Test(HasFlag(rbl->type.state, SUCCESS), "Roebling has state SUCCESS %s", State_ToChars(rbl->type.state));

    MemCtx_Free(m);
    return r;
}

status RoeblingStartStop_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2)); 

    String *s = String_Init(m, STRING_EXTEND);
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, NULL, s, Found, NULL); 

    char *cstr = "Hi how are you ";
    Roebling_AddBytes(rbl, bytes(cstr), strlen(cstr));
    Roebling_Run(rbl);

    cstr = "today?\n";
    Roebling_AddBytes(rbl, bytes(cstr), strlen(cstr));
    Roebling_Run(rbl);

    s = Range_Copy(m, &(rbl->range));
    /*
    Test(String_EqualsBytes(s, bytes("Hi how are you today?")), "String equals 'Hi how are you today?', have '%s'", (char *)s->bytes);
    */
    r |= Test((rbl->type.state & SUCCESS) != 0, "Roebling has state SUCCESS, have '%s'", State_ToChars(rbl->type.state));

    MemCtx_Free(m);
    return r;
}
