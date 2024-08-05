#include <external.h>
#include <caneka.h>
#include <proto/http.h>

enum test_enum {
    _START = 1000,
    ONE,
    TWO,
    THREE,
};

status SetWord1(MemHandle *m, Abstract *a){
   Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
   Lookup *lk = Lookup_Make(rbl->m, _START, NULL, NULL); 
   Lookup_Add(rbl->m, lk, TYPE_RANGE, 
        (void *)String_Make(rbl->m, bytes("ONE")));
   Lookup_Add(rbl->m, lk, TYPE_RANGE, 
        (void *)String_Make(rbl->m, bytes("TWO")));
   Lookup_Add(rbl->m, lk, TYPE_RANGE, 
        (void *)String_Make(rbl->m, bytes("THREE")));

   Roebling_ResetPatterns(rbl);
   return Roebling_SetLookup(rbl, lk); 
}

status Roebling_Tests(MemCtx *gm){

    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    String *s = NULL; 

    Roebling *rbl = NULL;
    Span *parsers_pmk = Span_Make(m);
    Span_Add(parsers_pmk, (Abstract *)Maker_Wrapped((MemHandle *)m, (Maker)SetWord1)); 
    Roebling_Make(m, TYPE_ROEBLING, parsers_pmk, String_Init(m, STRING_EXTEND), NULL); 

    return r;
}
