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

ProtoDef *HttpProtoDef_Make(MemCtx *m){
    Lookup *methods = Lookup_Make(m, _TYPE_HTTP_START, populateMethods, NULL);
    Span *parsers_rp = (Span *)Span_From(m, 9, 
        Parser_Method, Parser_Space, Parser_Path, Parser_HttpV, Parser_EndNl, 
        Parser_Mark,
        Parser_HColon, Parser_Space, Parser_HEndNl);

    return ProtoDef_Make(m, TYPE_HTTP_PROTODEF,
        (Maker)HttpReq_Make,
        (Maker)HttpProto_Make,
        parsers_rp,
        NULL,
        methods,
        NULL
    ); 
}
