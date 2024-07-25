#include <external.h>
#include <caneka.h>

status XmlCtx_Open(XmlCtx *ctx, String *tagName){
    Mess *ms = Mess_Make(ctx->m);
    ms->value = Hashed_Make(ctx->m, (Abstract *)tagName);
    if(ctx->current == NULL){
        Mess_Append(ms, ctx->root);
    }else{
        Mess_Append(ms, ctx->current->parent);
    }
    ctx->count++;
    ctx->current = ms;
    return SUCCESS;
}

status XmlCtx_SetAttr(XmlCtx *ctx, String *attName){
    printf("Setting Atts\n");
    Hashed *h = Hashed_Make(ctx->m, (Abstract *)attName);
    if(ctx->current->atts == NULL){
        ctx->current->atts = h;
    }else{
        Hashed *_h = ctx->current->atts;
        while(_h->next != NULL){
            _h = _h->next;
        }
        _h->next = h;
    }
    return SUCCESS;
}

status XmlCtx_Close(XmlCtx *ctx, String *tagName){
    ctx->current = ctx->current->parent;
    if(ctx->current == ctx->root){
        ctx->type.state = SUCCESS;
    }else{
        ctx->type.state = PROCESSING;
    }
    return ctx->type.state;
}

XmlCtx *XmlCtx_Make(MemHandle *mh, void *source){
    MemCtx *m = MemCtx_FromHandle(mh);
    XmlCtx *ctx = (XmlCtx*)MemCtx_Alloc(m, sizeof(XmlCtx));
    ctx->m = m;
    ctx->root = Mess_Make(ctx->m);
    ctx->type.of = TYPE_XMLCTX;
    return ctx;
}
