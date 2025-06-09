#include <external.h>
#include <caneka.h>

static Lookup *fmtToHtmlLookup = NULL;

static i64 headerFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = ctx->it.value;
    Str *s = Str_Make(ctx->m, 1+MAX_BASE10);
    if((flags & (TRANSP_OPEN|TRANSP_CLOSE)) && a->type.of == TYPE_NODE){
        Node *nd = (Node *)a;
        StrVec *v = (StrVec *)Table_Get(nd->atts, (Abstract *)I16_Wrapped(ctx->m, FORMATTER_INDENT));
        Str_Add(s, (byte *)"H", 1);
        if(v != NULL){
            Str_AddI64(s, v->total);
        }
    }
    if(flags & TRANSP_OPEN){
        total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
    }
    if(flags & TRANSP_BODY){
        total += ToS(ctx->sm, a, 0, ZERO);
    }
    if(flags & TRANSP_CLOSE){
        total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
        Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    return total;
}

static i64 paragraphFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = ctx->it.value;
    Str *s = Str_CstrRef(ctx->m, "P");
    if(flags & TRANSP_OPEN){
        total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
    }
    if(flags & TRANSP_BODY){
        total += ToS(ctx->sm, a, 0, ZERO);
    }
    if(flags & TRANSP_CLOSE){
        total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
        Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    return total;
}

status Fmt_ToHtml(Stream *sm, Mess *mess){
    if(fmtToHtmlLookup == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "FmtToHtml no initialized", NULL);
        return ERROR;
    }

    TranspCtx *ctx = TranspCtx_Make(sm->m, sm, fmtToHtmlLookup);
    mess->transp = ctx;
    ctx->type.state |= DEBUG;

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
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_PARAGRAPH, paragraphFunc);
        r = SUCCESS;
    }
    return r;
}

