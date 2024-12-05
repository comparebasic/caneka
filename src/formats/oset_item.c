#include <external.h>
#include <caneka.h>

Abstract *Abs_FromOsetItem(MemCtx *m, FmtCtx *o, FmtDef *def, FmtItem *item){
    if(def == NULL){
        Fatal("OsetDef not found", TYPE_OSET);
    }
    return def->from(m, def, o, item->key, (Abstract *)item->content);
}
