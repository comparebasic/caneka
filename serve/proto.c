#include <external.h>
#include <caneka.h>

char *Proto_ToChars(Proto *proto){
    return "Proto";
}

ProtoDef *ProtoDef_Make(MemCtx *m, cls type, Maker req_mk, Maker proto_mk , Span *parsers_pmk, Lookup *handlers, Lookup *methods, Abstract *virt){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->parsers_pmk = parsers_pmk;
    def->type.state = (status)type;
    def->handlers = handlers;
    def->methods = methods;
    def->virt = virt;
    def->req_mk = req_mk;
    def->proto_mk = proto_mk;

    return def;
}
