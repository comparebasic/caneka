#include <external.h>
#include <caneka.h>

static char *toLog(Req *req){
    IoProto *proto = (IoProto *)req->proto;
    String *s = String_Init(req->m, STRING_EXTEND);
    String_AddBytes(req->m, s, bytes("IoProto<"), strlen("IoProto<"));
    char *state = State_ToChars(req->type.state);
    String_AddBytes(req->m, s, bytes(state), strlen(state));
    char *cstr = " shelf:\"";
    String_AddBytes(req->m, s, bytes(cstr), strlen(cstr));
    String_Add(req->m, s, req->in.shelf);
    String_AddBytes(req->m, s, bytes("\">"), 2);

    return (char *)s->bytes;
}

Proto *IoProto_Make(MemCtx *m, ProtoDef *def){
    IoProto *p = (IoProto *)MemCtx_Alloc(m, sizeof(IoProto));
    p->type.of = TYPE_IO_PROTO;
    p->toLog = toLog;
    p->headers_tbl = Span_Make(m, TYPE_TABLE);
    p->methods = def->methods;

    return (Proto *)p;
}

ProtoDef *IoProtoDef_Make(MemCtx *m, Maker reqMake){
    Stack(bytes("IoProtoDef_Make"), NULL);
    Lookup *lk = Lookup_Make(m, _IO_START, NULL, NULL); 
    Lookup_Add(m, lk, IO_RECV, 
         (void *)String_Make(m, bytes("RECV")));
    Lookup_Add(m, lk, IO_RUN, 
         (void *)String_Make(m, bytes("RUN")));
    Lookup_Add(m, lk, IO_DONE, 
         (void *)String_Make(m, bytes("DONE")));

    Return ProtoDef_Make(m, TYPE_IO_PROTODEF,
        (Maker)reqMake,
        (Maker)IoProto_Make,
        lk
    ); 
}
