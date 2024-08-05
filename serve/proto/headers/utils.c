#include <external.h>
#include <caneka.h>

Abstract *Hdr_IntMk(MemCtx *m, Abstract *a){
    m = MemCtx_FromHandle((MemHandle *)m);
    String *s = (String *)asIfc(a, TYPE_STRING);
    return (Abstract *)Int_Wrapped(m, Int_FromString(s));
}

int Hdr_GetInt(Span *tbl, String *key){
    Single *hdr = (Single *) Table_Get(tbl, (Abstract *)key);
    if(hdr != NULL && hdr->type.of == TYPE_WRAPPED_UTIL){
        return (int) ((Single *)hdr)->val.value;
    }
    return 0;
}
