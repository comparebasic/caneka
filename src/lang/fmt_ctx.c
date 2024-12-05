#include <external.h>
#include <caneka.h>

FmtCtx *FmtCtx_Make(MemCtx *m){
    FmtCtx *fmt = (FmtCtx *)MemCtx_Alloc(m, sizeof(FmtCtx));
    fmt->type.of = TYPE_FMT_CTX;
    fmt->m = m;
    return fmt;
}
