#include <external.h>
#include <caneka.h>
#include <proto/http.h>

enum test_enum {
    _START = 1000,
    ONE,
    TWO,
    THREE,
};

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
   return Roebling_SetLookup(rbl, lk); 
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

    return r;
}
