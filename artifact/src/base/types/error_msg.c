#include <external.h>
#include <caneka.h>

i64 ErrorMsg_ToStream(Stream *sm, ErrorMsg *msg){
    i64 total = 0;
    Str *s = Str_CstrRef(sm->m, "Error: ");
    total += Stream_Bytes(sm, s->bytes, s->length);
    total += Stream_Bytes(sm, (byte *)msg->func, strlen(msg->func));
    total += Stream_Bytes(sm, (byte *)":", 1);
    total += Stream_Bytes(sm, (byte *)msg->file, strlen(msg->file));
    total += Stream_Bytes(sm, (byte *)":", 1);
    s = Str_FromI64(sm->m, msg->lineno); 
    total += Stream_Bytes(sm, s->bytes, s->length);
    total += Stream_Bytes(sm, (byte *)": ", 2);
    total += Fmt(sm, msg->fmt, msg->args);
    return total;
}
