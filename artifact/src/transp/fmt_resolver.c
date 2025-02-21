#include <external.h>
#include <caneka.h>

FmtResolver *FmtResolver_Make(MemCtx *m){
    FmtResolver *rslv = MemCtx_Alloc(m, sizeof(FmtResolver));
    rslv->type.of = TYPE_FMT_RESOLVER;

    rslv->byId = Chain_Make(m, NULL);
    rslv->byName = TableChain_Make(m, NULL);
    rslv->byAlias = TableChain_Make(m, NULL);

    return rslv;
}
