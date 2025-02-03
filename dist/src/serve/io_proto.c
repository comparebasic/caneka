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

Proto *IoProto_Make(MemCtx *m, ProtoDef *def, word flags){
    IoProto *p = (IoProto *)MemCtx_Alloc(m, sizeof(IoProto));
    p->type.of = TYPE_IO_PROTO;
    p->toLog = toLog;
    p->def = def;

    return (Proto *)p;
}

ProtoDef *IoProtoDef_Make(MemCtx *m, Maker reqMake){
    return ProtoDef_Make(m, TYPE_IO_PROTODEF,
        (FlagMaker)reqMake,
        NULL,
        (FlagMaker)IoProto_Make); 
}
