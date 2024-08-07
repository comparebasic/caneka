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

status SetWord2(Abstract *a){
   return NOOP; 
}

status Roebling_Tests(MemCtx *gm){

    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    String *s = NULL; 

    Roebling *rbl = NULL;
    Span *parsers_do = Span_Make(m);
    Span_Add(parsers_do, (Abstract *)Do_Wrapped((MemHandle *)m, (DoFunc)SetWord1)); 
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, String_Init(m, STRING_EXTEND), NULL); 

    Single *dof = as(Span_Get(rbl->parsers_do, 0), TYPE_WRAPPED_DO);
    dof->val.dof((MemHandle *)rbl);

    Debug_Print((void *)rbl, 0, "Roebling: ", COLOR_CYAN, TRUE);
    printf("\n");

    r |= Test(rbl->matches.values->nvalues == 3, "Roebling has three match values loaded up");

    s = String_Make(m, "TWO for the weekend\n");
    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_Run(rbl);

    Match *mt = Roebling_GetMatch(rbl);
    s = Range_Copy(rbl->m, &(rbl->range));
    int idx = Roebling_GetMatchIdx(rbl);

    r |= Test(String_EqualsBytes(mt->def.s, bytes("TWO")), "Match equals expected");
    r |= Test(String_EqualsBytes(s, bytes("TWO")), "Content equals expected");
    r |= Test(idx = 1, "Match Idx equals expected");

    return r;
}
