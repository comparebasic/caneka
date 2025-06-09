#include <external.h>
#include <caneka.h>

static Lookup *fmtToHtmlLookup = NULL;

static i64 headerFunc(TranspCtx *ctx){
    Node *nd = as(ctx->it.value, TYPE_NODE);
    i64 total = 0;
    StrVec *v = Table_Get(nd->atts, (Abstract *)I16_Wrapped(m, FORMATTER_INDENT));
    Str *s = Str_Make(ctx->m, 1+MAX_BASE10);
    Str_Add(m, (byte *)"H", 1);
    if(v != NULL){
        Str_AddI64(s, v->total);
    }
    total += Tag_Out(ctx->sm, s, ZERO);
    total += ToS(ctx->sm, nd->child, 0, ZERO);
    total += Tag_Out(ctx->sm, s, TAG_CLOSE);
    return total;
}

status FmtToHtml_Init(MemCtx *m){
    if(fmtToHtmlLookup == NULL){
        fmtToHtmlLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_INDENT, headerFunc);
    }
}

status Fmt_ToHtml(Stream *sm, Mess *mess){
    if(fmtToHtmlLookup == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "FmtToHtml no initialized", NULL);
        return ERROR;
    }

    TranspCtx *ctx = TranspCtx_Make(sm->m, sm, fmtToHtmlLookup);

    Iter_Setup(&ctx->it, ctx->it.p, SPAN_OP_SET, 0);
    comp->it.value = (Abstract *)mess->root;
    Iter_Query(&ctx->it);

    return Transp(ctx);
}
