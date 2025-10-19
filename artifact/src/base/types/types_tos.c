#include <external.h>
#include <caneka.h>

static i64 ErrorMsg_Print(Stream *sm, Abstract *a, cls type, word flags){
    if(a == NULL){
        Str *s = Str_CstrRef(sm->m, "Error");
        return Stream_Bytes(sm, s->bytes, s->length);
    }
    i64 total = 0;
    ErrorMsg *msg = (ErrorMsg *)as(a, TYPE_ERROR_MSG);
    if(flags & (MORE|DEBUG)){
        total += Fmt(sm, "Error $:$:$ - ", msg->lineInfo);
    }
    total += Fmt(sm, (char *)msg->fmt->bytes, msg->args);
    return total;
}

status Types_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ERROR_MSG, (void *)ErrorMsg_Print);
    return r;
}
