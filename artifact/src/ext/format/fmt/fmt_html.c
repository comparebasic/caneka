#include <external.h>
#include <caneka.h>

static Lookup *fmtToHtmlLookup = NULL;

static i64 headerFunc(TranspCtx *ctx){
    Node *nd = (Node *)as(ctx->it.value, TYPE_NODE);
    i64 total = 0;
    StrVec *v = (StrVec *)Table_Get(nd->atts, (Abstract *)I16_Wrapped(ctx->m, FORMATTER_INDENT));
    Str *s = Str_Make(ctx->m, 1+MAX_BASE10);
    Str_Add(s, (byte *)"H", 1);
    if(v != NULL){
        Str_AddI64(s, v->total);
    }
    total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
    total += ToS(ctx->sm, nd->child, 0, ZERO);
    total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
    return total;
}

status Fmt_ToHtml(Stream *sm, Mess *mess){
    if(fmtToHtmlLookup == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "FmtToHtml no initialized", NULL);
        return ERROR;
    }

    TranspCtx *ctx = TranspCtx_Make(sm->m, sm, fmtToHtmlLookup);
    mess->transp = ctx;

    Iter_Setup(&ctx->it, ctx->it.p, SPAN_OP_SET, 0);
    ctx->it.value = (Abstract *)mess->root;
    Iter_Query(&ctx->it);
    i64 total = 0;
    while((ctx->type.state & (SUCCESS|ERROR|ERROR)) == 0){
        total += Transp(ctx);
    }

    return total;
}

status FmtToHtml_Init(MemCh *m){
    status r = READY;
    if(fmtToHtmlLookup == NULL){
        fmtToHtmlLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_INDENT, headerFunc);
        r = SUCCESS;
    }
    return r;
}

