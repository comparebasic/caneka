#include <external.h>
#include <caneka.h>

HandlerDef *HandlerDef_Make(MemCh *m){
    HandlerDef *def = MemCh_AllocOf(m, sizeof(HandlerDef), TYPE_HANDLER_DEF);
    def->type.of = TYPE_HANDLER_DEF;
    return def;
}
