#include <external.h>
#include <filestore.h>

char *Proto_ToChars(Proto *proto){
    return "Proto";
}

ProtoDef *ProtoDef_Make(MemCtx *m, Lookup *handlers, Lookup *methods, Virtual *virt){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->handlers = handlers;
    def->methods = methods;
    def->virt = virt;
    return def;
}
