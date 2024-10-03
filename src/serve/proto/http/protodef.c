#include <external.h>
#include <caneka.h>
#include <proto/http.h>

ProtoDef *HttpProtoDef_Make(MemCtx *m, Lookup *handlers){
    return ProtoDef_Make(m, TYPE_HTTP_PROTODEF,
        (Maker)HttpReq_Make,
        (Maker)HttpProto_Make,
        handlers
    ); 
}
