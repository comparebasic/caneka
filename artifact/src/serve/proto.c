#include <external.h>
#include <caneka.h>

char *Proto_ToChars(Proto *proto){
    return "Proto";
}

ProtoDef *ProtoDef_Make(MemCtx *m, cls type, FlagMaker req_mk, FlagMaker reqFree_mk, FlagMaker proto_mk){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->reqType = type;
    def->req_mk = req_mk;
    def->reqFree_mk = reqFree_mk;
    def->proto_mk = proto_mk;

    return def;
}
