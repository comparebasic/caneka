#include <external.h>
#include <caneka.h>

DefMakerFunc HandlerDefFunc_ByVec(MemCh *m, StrVec *proto){
    Single *sg = Table_Get(ServeProtoTable, proto);
    if(sg == NULL){
        void *ar[] = {proto, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error proto not found @^0\n", ar);
        return NULL;
    }

    return (DefMakerFunc)sg->val.ptr;
}

HandlerDef *HandlerDef_Make(MemCh *m){
    HandlerDef *def = MemCh_AllocOf(m, sizeof(HandlerDef), TYPE_HANDLER_DEF);
    def->type.of = TYPE_HANDLER_DEF;
    return def;
}
