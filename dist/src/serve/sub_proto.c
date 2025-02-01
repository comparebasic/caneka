#include <external.h>
#include <caneka.h>

static char *toLog(Req *req){
    SubProto *proto = (SubProto *)req->proto;
    String *s = String_Init(req->m, STRING_EXTEND);
    String_AddBytes(req->m, s, bytes("SubProto<"), strlen("SubProto<"));
    char *state = State_ToChars(req->type.state);
    String_AddBytes(req->m, s, bytes(state), strlen(state));
    char *cstr = " shelf:\"";
    String_AddBytes(req->m, s, bytes(cstr), strlen(cstr));
    String_Add(req->m, s, req->in.shelf);
    String_AddBytes(req->m, s, bytes("\">"), 2);

    return (char *)s->bytes;
}

Proto *SubProto_Make(MemCtx *m, ProtoDef *def, word flags){
    SubProto *p = (SubProto *)MemCtx_Alloc(m, sizeof(SubProto));
    p->type.of = TYPE_SUB_PROTO;
    p->toLog = toLog;

    return (Proto *)p;
}

ProtoDef *SubProtoDef_Make(MemCtx *m, Maker reqMake){
    return ProtoDef_Make(m, TYPE_SUB_PROTODEF,
        (FlagMaker)reqMake,
        (FlagMaker)SubProto_Make
    ); 
}
