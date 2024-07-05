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
    return ProtoDef_Make(m, TYPE_HTTP_PROTODEF, NULL, methods, NULL); 
}
