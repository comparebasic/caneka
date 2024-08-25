#include <external.h>
#include <caneka.h>

enum test_enum {
    _START = 1000,
    ONE,
    TWO,
    THREE,
};

status WordFound(MemHandle *mh){
    if(DEBUG_ROEBLING){
        Roebling *rbl = as(mh, TYPE_ROEBLING);
        Match *mt = Roebling_GetMatch(rbl);
        Debug_Print((void *)mt, 0, "found mt: ", COLOR_YELLOW, TRUE);
        printf("\n");
        String *s = Range_Copy(rbl->m, &(rbl->range));
        Debug_Print((void *)s, 0, "found s: ", COLOR_YELLOW, TRUE);
        printf("\n");
    }
    return SUCCESS;
}

status RestFound(MemHandle *mh){
    if(DEBUG_ROEBLING){
        Roebling *rbl = as(mh, TYPE_ROEBLING);
        printf("\n");
        String *s = Range_Copy(rbl->m, &(rbl->range));
        Debug_Print((void *)s, 0, "found rest: ", COLOR_YELLOW, TRUE);
        printf("\n");
    }
    return SUCCESS;
}

status SetWord1(Abstract *a){
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    Lookup *lk = Lookup_Make(rbl->m, ONE, NULL, NULL); 
    Lookup_Add(rbl->m, lk, ONE, 
         (void *)String_Make(rbl->m, bytes("ONE")));
    Lookup_Add(rbl->m, lk, TWO, 
         (void *)String_Make(rbl->m, bytes("TWO")));
    Lookup_Add(rbl->m, lk, THREE, 
         (void *)String_Make(rbl->m, bytes("THREE")));
 
    Roebling_ResetPatterns(rbl);
    rbl->dispatch = WordFound;
    return Roebling_SetLookup(rbl, lk); 
}

status SetBravo(Abstract *a){
    return NOOP; 
}

static word text[] = {PAT_IGNORE|PAT_ANY, ' ', ' ',TEXT_DEF};
static word nl[] = {NL_DEF};

status SetWord2(Abstract *a){
    status r = READY;

    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, (PatCharDef *)text);
    r |= Roebling_SetKOPattern(rbl, (PatCharDef *)nl);
    rbl->dispatch = RestFound;
    return r; 
}

status Roebling_Tests(MemCtx *gm){

    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    String *s = NULL; 

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped((MemHandle *)m, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped((MemHandle *)m, (DoFunc)SetWord2)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, 0, String_Init(m, STRING_EXTEND), NULL); 

    Single *dof = as(Span_Get(rbl->parsers_do, 0), TYPE_WRAPPED_DO);
    dof->val.dof((MemHandle *)rbl);

    r |= Test(rbl->matches.values->nvalues == 3, "Roebling has three match values loaded up");

    MemCtx_Free(m);
    return r;
}

enum rbl_test_marks {
    RBL_TEST_START = 0,
    RBL_TEST_BRAVO,
    RBL_TEST_END,
};

status RoeblingRun_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Roebling *rbl = NULL;
    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(mh, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(mh, (DoFunc)SetWord2)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, 0, String_Init(m, STRING_EXTEND), NULL); 

    String *s = NULL; 
    s = String_Make(m, bytes("TWO for the weekend\n"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_Run(rbl);

    Match *mt = Roebling_GetMatch(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    int idx = Roebling_GetMatchIdx(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(mt->def.s, bytes("TWO")), "Match equals expected");
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals expected, have %s", s->bytes);
    r |= Test(idx = 1, "Match Idx equals expected");
    r |= Test(rbl->type.state == NEXT, "Roebling has state NEXT");

    Roebling_Run(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("for the weekend")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test(HasFlag(rbl->type.state, NEXT), "Roebling has state NEXT");

    Roebling_Run(rbl);
    r |= Test(HasFlag(rbl->type.state, SUCCESS), "Roebling has state SUCCESS");

    MemCtx_Free(m);
    return r;
}

status RoeblingMark_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Roebling *rbl = NULL;
    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_START)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(mh, (DoFunc)SetWord1)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(mh, (DoFunc)SetWord2)); 
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_BRAVO)); 
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(mh, (DoFunc)SetBravo)); 
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBL_TEST_END)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, RBL_TEST_START, String_Init(m, STRING_EXTEND), NULL); 

    String *s = NULL; 
    s = String_Make(m, bytes("TWO for the weekend\n"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_Run(rbl);

    Match *mt = Roebling_GetMatch(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    int idx = Roebling_GetMatchIdx(rbl);
    s = Range_Copy(m, &(rbl->range));
    r |= Test(String_EqualsBytes(mt->def.s, bytes("TWO")), "Match equals expected");
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals expected, have %s", s->bytes);
    r |= Test(idx = 1, "Match Idx equals expected");
    r |= Test(rbl->type.state == NEXT, "Roebling has state NEXT");

    Roebling_Run(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    r |= Test(String_EqualsBytes(s, bytes("for the weekend")), "Roebling has captured the rest of the line: '%s'", s->bytes);
    r |= Test(HasFlag(rbl->type.state, NEXT), "Roebling has state NEXT");

    Roebling_Run(rbl);
    r |= Test(HasFlag(rbl->type.state, SUCCESS), "Roebling has state SUCCESS");

    MemCtx_Free(m);
    return r;
}
