#include <external.h>
#include <caneka.h>

status XmlCtx_Open(XmlCtx *ctx, String *tagName){
    Mess *ms = Mess_Make(ctx->m);
    ms->name = tagName;
    if(ctx->parent == NULL){
        ctx->parent = ctx->root;
    }
    Mess_Append(ctx->parent, ms);
    ctx->count++;
    ctx->current = ms;
    return SUCCESS;
}

status XmlCtx_SetAttr(XmlCtx *ctx, String *attName){
    if(ctx->current->atts == NULL){
        ctx->current->atts = Span_Make(ctx->m, TYPE_TABLE);
    }

    return Table_SetKey(ctx->current->atts, (Abstract *)attName);
}

status XmlCtx_BodyAppend(XmlCtx *ctx, String *body){
    Mess *ms = Mess_Make(ctx->m);
    ms->body = body;
    Mess_Append(ctx->parent, ms);
    return SUCCESS;
}

status XmlCtx_SetAttrValue(XmlCtx *ctx, Abstract *value){
    if(ctx->current->atts == NULL){
        ctx->current->atts = Span_Make(ctx->m, TYPE_TABLE);
    }
    if(Table_SetValue(ctx->current->atts, (Abstract *)value) != NULL){
        return SUCCESS;
    }
    return ERROR;
}

status XmlCtx_TagClosed(XmlCtx *ctx){
    ctx->parent = ctx->current;
    return ctx->type.state;
}

status XmlCtx_Close(XmlCtx *ctx, String *tagName){
    printf("closing tag\n");
    if(ctx->current == ctx->root->firstChild || ctx->parent == ctx->root->firstChild){
        ctx->type.state = SUCCESS;
    }else{
        ctx->type.state = PROCESSING;
    }
    ctx->current = ctx->parent;
    ctx->parent = ctx->current->parent;
    return ctx->type.state;
}

XmlCtx *XmlCtx_Make(MemCtx *m){
    XmlCtx *ctx = (XmlCtx*)MemCtx_Alloc(m, sizeof(XmlCtx));
    ctx->m = m;
    ctx->root = Mess_Make(ctx->m);
    ctx->parent = ctx->root;
    ctx->type.of = TYPE_XMLCTX;
    return ctx;
}
