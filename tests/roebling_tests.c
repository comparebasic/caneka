#include <external.h>
#include <caneka.h>
#include <proto/http.h>

static status populateMethods(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_METHOD_GET, (Abstract *)String_Make(m, bytes("GET")));
    r |= Lookup_Add(m, lk, TYPE_METHOD_POST, (Abstract *)String_Make(m, bytes("POST")));
    r |= Lookup_Add(m, lk, TYPE_METHOD_SET, (Abstract *)String_Make(m, bytes("SET")));
    r |= Lookup_Add(m, lk, TYPE_METHOD_UPDATE, (Abstract *)String_Make(m, bytes("UPDATE")));
    return r;
}

static status ParseComplete(Parser *prs, Range *range, void *source){
    MemCtx *m = (MemCtx *)source;
    Debug_Print((void *)Parser_GetMatchKey(prs), 0, "Completed Match key: ", COLOR_YELLOW, TRUE);
    printf("\n");
    Debug_Print((void *)Range_Copy(m, range), 0, "Completed Range: ", COLOR_YELLOW, TRUE);
    printf("\n");
    return prs->type.state;
}

static Parser *makeOneTwoThree(Roebling *rbl){
    MemCtx *m = rbl->m;
    Match **onetwothree = (Match **)Array_MakeFrom(m, 3, 
        Match_Make(m, String_Make(m, bytes("one")), ANCHOR_START),
        Match_Make(m, String_Make(m, bytes("two")), ANCHOR_START),
        Match_Make(m, String_Make(m, bytes("three")), ANCHOR_START)
    );

    return Parser_MakeMulti(m, onetwothree, ParseComplete);
}

status Roebling_Tests(MemCtx *gm){

    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    String *s = NULL; 
    Lookup *methods = Lookup_Make(m, _TYPE_HTTP_START, populateMethods, NULL);

    s = String_Make(m, bytes("one and a bunch!"));
    Span *parsers_pmk = (Span *)Span_From(m, 1, makeOneTwoThree);
    Roebling *rbl = Roebling_Make(m, TYPE_HTTP_PARSER, 
        parsers_pmk, s, (Abstract *)m);  

    Roebling_Run(rbl);

    Debug_Print((void *)rbl, 0, "Roebling HTTP: ", COLOR_CYAN, TRUE);
    printf("\n");

    s = String_Make(m, bytes("two and a half" ));
    parsers_pmk = (Span *)Span_From(m, 1, makeOneTwoThree);
    rbl = Roebling_Make(m, TYPE_HTTP_PARSER, parsers_pmk, s, (Abstract *)m);  

    Roebling_Run(rbl);

    /*
    r |= Test(s->type.of == TYPE_STRING_FIXED, 
        "Expect string to have fixed type %s found %s", 
            Class_ToString(TYPE_STRING_FIXED), Class_ToString(s->type.of));
            */

    return r;
}
