#include <external.h>
#include <caneka.h>
#include <rblsh.h>

status RblShSuper_Capture(word rkey, int matchIdx, String *s, Abstract *source){
    RblShSuper *super = (RblShSuper *)as(source, TYPE_RBLSH_SUPER);
    return SUCCESS;
}

RblShSuper *RblShSuper_Make(MemCtx *m, RblShCtx *ctx){
    RblShSuper *super = MemCtx_Alloc(m, sizeof(RblShSuper));
    super->type.of = TYPE_RBLSH_SUPER;
    Span_Add(ctx->supers, (Abstract *)super);

    return super;
}
