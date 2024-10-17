#include <external.h>
#include <caneka.h>

char *Proto_ToChars(Proto *proto){
    return "Proto";
}

ProtoDef *ProtoDef_Make(MemCtx *m, cls type, Maker req_mk, Maker proto_mk, Lookup *methods){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->reqType = type;
    def->req_mk = req_mk;
    def->proto_mk = proto_mk;
    def->methods = methods;

    return def;
}
