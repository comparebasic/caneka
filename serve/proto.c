#include <external.h>
#include <caneka.h>

char *Proto_ToChars(Proto *proto){
    return "Proto";
}

ProtoDef *ProtoDef_Make(MemCtx *m, cls type, Lookup *handlers, Lookup *methods, Abstract *virt){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->type.state = (status)type;
    def->handlers = handlers;
    def->methods = methods;
    def->virt = virt;
    return def;
}
