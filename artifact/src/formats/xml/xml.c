#include <external.h>
#include <caneka.h>

status XmlCtx_Open(XmlCtx *ctx, String *tagName){
    Span *tagTbl = ctx->set->tagTbl;
    Single *tag = (Single *)Table_Get(tagTbl, (Abstract *)tagName);
    word tagIdx = 0;
    if(tag == NULL){
        tagIdx = (word)Table_SetIdxEntry(tagTbl, (Abstract *)tagName);
    }else{
        tagIdx = (word)tag->val.value; 
    }

    Mess *ms = Mess_Make(ctx->m, tagIdx);
    ms->atts = (Span *)OrdTable_Make(ctx->m);
    Mess_Append(ctx->set, ctx->parent, NULL, (Abstract *)ms);
    ctx->count++;
    ctx->current = ms;
    return SUCCESS;
}

status XmlCtx_SetAttr(XmlCtx *ctx, String *attName){
    return Table_SetKey(ctx->current->atts, (Abstract *)attName);
}

status XmlCtx_BodyAppend(XmlCtx *ctx, String *body){
    return Mess_Append(ctx->parent, NULL, (Abstract *)body);
}

status XmlCtx_SetAttrValue(XmlCtx *ctx, Abstract *value){
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
    if(ctx->current == ctx->set->root || ctx->set->root){
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
    ctx->set = MessSet_Make(ctx->m);
    ctx->parent = ctx->set->root;
    ctx->type.of = TYPE_XMLCTX;
    return ctx;
}
