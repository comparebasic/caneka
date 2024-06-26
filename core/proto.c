#include <external.h>
#include <filestore.h>

ProtoDef ProtoDef_Make(MemCtx *m, RlbLookup *handlers, RlbLookup *methods, Virtual *virt){
    ProtoDef *def = (ProtoDef *)MemCtx_Alloc(m, sizeof(ProtoDef));
    def->type.of = TYPE_PROTODEF;
    def->handlers = handlers;
    def->methods = methods;
    def->virt = virt;
    return virt;
}
