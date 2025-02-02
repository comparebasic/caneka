#include <external.h>
#include <caneka.h>

static char *toLog(Req *req){
    SubProto *proto = (SubProto *)req->proto;
    String *s = String_Init(req->m, STRING_EXTEND);
    String_AddBytes(req->m, s, bytes("SubProto<"), strlen("SubProto<"));
    char *state = State_ToChars(req->type.state);
    String_AddBytes(req->m, s, bytes(state), strlen(state));
    state = ProcIoSet_FlagsToChars(req->type.state);
    String_AddBytes(req->m, s, bytes(state), strlen(state));
    char *cstr = " pid/ret:\"";
    String_AddBytes(req->m, s, bytes(cstr), strlen(cstr));
    String_AddInt(req->m, s, proto->procio->pd.pid);
    String_AddBytes(req->m, s, bytes("/"), 1);
    String_AddInt(req->m, s, proto->procio->pd.code);
    cstr = " shelf-length:\"";
    String_AddBytes(req->m, s, bytes(cstr), strlen(cstr));
    String_AddInt(req->m, s, String_Length(req->in.shelf));
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
