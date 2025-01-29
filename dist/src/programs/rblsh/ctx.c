#include <external.h>
#include <caneka.h>
#include <rblsh.h>

status RblShCtx_Capture(word rkey, int matchIdx, String *s, Abstract *source){
    RblShCtx *ctx = (RblShCtx *)as(source, TYPE_RBLSH_CTX);
    for(int i = 0; i < s->length; i++){
        byte b = s->bytes[i];
        printf("Recieved key %c\n", b);
        if(b == KEY_INT || b == KEY_HUP){
            ctx->sctx->serving = FALSE; 
        }
    }
    return SUCCESS;
}

Abstract *RblShCtx_GetVar(Abstract *store, Abstract *key){
    RblShCtx *ctx = (RblShCtx *)as(store, TYPE_RBLSH_CTX);
    return Table_Get(ctx->env, key);
}

RblShCtx *RblShCtx_Make(MemCtx *m){
    RblShCtx *ctx = MemCtx_Alloc(m, sizeof(RblShCtx));
    ctx->type.of = TYPE_RBLSH_CTX;

    ctx->cash = Cash_Make(m, RblShCtx_GetVar, (Abstract *)ctx);
    ctx->cash->rbl = NULL;

    char buff[PATH_BUFFLEN];
    char *path = getcwd(buff, PATH_BUFFLEN);
    ctx->cwd.s = String_Make(m, bytes(path));
    ctx->cwd.p = String_SplitToSpan(m, ctx->cwd.s, String_Make(m, bytes("/")));

    return ctx;
}
