#include <external.h>
#include <caneka.h>

status XmlCtx_Open(XmlCtx *ctx, String *tagName){
    Mess *ms = Mess_Make(ctx->m);
    ms->value = Hashed_Make(ctx->m, (Abstract *)tagName);
    if(ctx->parent == NULL){
        ctx->parent = ctx->root;
    }
    Mess_Append(ms, ctx->parent);
    ctx->count++;
    ctx->current = ms;
    return SUCCESS;
}

status XmlCtx_SetAttr(XmlCtx *ctx, String *attName){
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

status XmlCtx_SetAttrValue(XmlCtx *ctx, Abstract *value){
    Hashed *h = ctx->current->atts;
    while(h->next != NULL){
        h = h->next;
    }
    h->value = value;
    return SUCCESS;
}

status XmlCtx_TagClosed(XmlCtx *ctx){
    ctx->parent = ctx->current;
    ctx->current = NULL;
    return ctx->type.state;
}

status XmlCtx_Close(XmlCtx *ctx, String *tagName){
    if(ctx->current == ctx->root->firstChild){
        ctx->type.state = SUCCESS;
    }else{
        ctx->type.state = PROCESSING;
    }
    ctx->current = ctx->parent;
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
