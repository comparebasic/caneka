#include <external.h>
#include <caneka.h>

FmtDef *FmtDef_Make(MemCtx *m){
    FmtDef *fi =  MemCtx_Alloc(m, sizeof(FmtDef));
    fi->type.of = TYPE_FMT_DEF;
    return fi;
}
