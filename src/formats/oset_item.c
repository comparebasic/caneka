#include <external.h>
#include <caneka.h>

Abstract *Abs_FromOsetItem(MemCtx *m, Oset *o, OsetItem *oi){
    OsetDef *odef = oi->odef;
    if(odef == NULL){
        Fatal("OsetDef not found", TYPE_OSET);
    }
    return odef->fromOset(m, odef, o, oi->key, (Abstract *)oi->content);
}

OsetItem *OsetItem_Make(MemCtx *m, Oset *o){
    OsetItem *oi =  MemCtx_Alloc(m, sizeof(OsetItem));
    oi->type.of = TYPE_OSET_ITEM;
    return oi;
}
