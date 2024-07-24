#include <external.h>
#include <caneka.h>
XmlCtx *XmlCtx_Make(MemHandle *mh, void *source){
    MemCtx *m = MemCtx_FromHandle(mh);
    XmlCtx *ctx = (XmlCtx*)MemCtx_Alloc(m, sizeof(XmlCtx));
    ctx->m = m;
    ctx->type.of = TYPE_XMLCTX;
    return ctx;
}
