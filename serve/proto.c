#include <external.h>
#include <caneka.h>

char *Proto_ToChars(Proto *proto){
    return "Proto";
}

ProtoDef *ProtoDef_Make(MemCtx *m, cls type, Maker req_mk, Maker proto_mk, Abstract *source){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->type.state = (status)type;
    def->req_mk = req_mk;
    def->proto_mk = proto_mk;
    def->source = source;

    return def;
}
