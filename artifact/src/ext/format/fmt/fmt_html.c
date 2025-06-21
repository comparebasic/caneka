#include <external.h>
#include <caneka.h>

static Lookup *fmtToHtmlLookup = NULL;

static i64 tableFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = ctx->it.value;
    if(flags & TRANSP_OPEN){
        Str *s = Str_CstrRef(ctx->m, "TABLE");
        total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
        total += Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    if(flags & TRANSP_BODY){
        Relation *rel = (Relation *)as(a, TYPE_RELATION);
        Relation_ResetIter(rel);

        while((Relation_Next(rel) & END) == 0){
            if(rel->type.state & RELATION_ROW_START){
                Stream_Bytes(ctx->sm, (byte *)"  ", 2);
                if(rel->it.idx == 0){
                    Str *s = Str_CstrRef(ctx->m, "THEAD");
                    total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
                }
                Str *s = Str_CstrRef(ctx->m, "TR");
                total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
            }
            Str *s = Str_CstrRef(ctx->m, "TD");
            total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
            total += ToS(ctx->sm, rel->it.value, 0, ZERO);
            total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
            if(rel->type.state & RELATION_ROW_END){
                Str *s = Str_CstrRef(ctx->m, "TR");
                total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
                if(rel->it.idx+1 == rel->stride){
                    Str *s = Str_CstrRef(ctx->m, "THEAD");
                    total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
                }
                total += Stream_Bytes(ctx->sm, (byte *)"\n", 1);
            }
        }
    }
    if(flags & TRANSP_CLOSE){
        Str *s = Str_CstrRef(ctx->m, "TABLE");
        total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
        Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    return total;
}


static i64 bulletFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = ctx->it.value;
    if(flags & TRANSP_OPEN){
        Str *s = Str_CstrRef(ctx->m, "UL");
        total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
        Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    if(flags & TRANSP_BODY){
        Stream_Bytes(ctx->sm, (byte *)"  ", 2);
        Str *s = Str_CstrRef(ctx->m, "LI");
        total += Tag_Out(ctx->sm, (Abstract *)s, ZERO);
        total += ToS(ctx->sm, a, 0, ZERO);
        total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
        Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    if(flags & TRANSP_CLOSE){
        Str *s = Str_CstrRef(ctx->m, "UL");
        total += Tag_Out(ctx->sm, (Abstract *)s, TAG_CLOSE);
        Stream_Bytes(ctx->sm, (byte *)"\n", 1);
    }
    return total;
}

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

static i64 linkFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Node *nd = (Node *)as(ctx->it.value, TYPE_NODE);

    StrVec *text = (StrVec *)Table_Get(nd->atts, 
        (Abstract *)I16_Wrapped(ctx->m, FORMATTER_LABEL));
    StrVec *href = (StrVec *)Table_Get(nd->atts, 
        (Abstract *)I16_Wrapped(ctx->m, FORMATTER_URL));

    Abstract *args[] = {
        (Abstract *)href,
        (Abstract *)text,
    };
    return Fmt(ctx->sm, "<A href=\"$\">$</A>", args);
}

static i64 imageFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Node *nd = (Node *)as(ctx->it.value, TYPE_NODE);

    StrVec *label = (StrVec *)Table_Get(nd->atts, 
        (Abstract *)I16_Wrapped(ctx->m, FORMATTER_LABEL));
    StrVec *src = (StrVec *)Table_Get(nd->atts, 
        (Abstract *)I16_Wrapped(ctx->m, FORMATTER_URL));

    Abstract *args[] = {
        (Abstract *)label,
        (Abstract *)src,
    };
    return Fmt(ctx->sm, "<IMG alt=\"$\" src=\"$\" />", args);
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

static i64 tagFunc(TranspCtx *ctx, word flags){
    if((flags & TRANSP_CLOSE) == 0){
        Node *nd = (Node *)as(ctx->it.value, TYPE_NODE);
        StrVec *tag = (StrVec *)Table_Get(nd->atts, 
            (Abstract *)I16_Wrapped(ctx->m, FORMATTER_TAG));
        if(Equals((Abstract *)tag, (Abstract *)Str_CstrRef(ctx->m, "link"))){
            return linkFunc(ctx, flags);
        }else if(Equals((Abstract *)tag, (Abstract *)Str_CstrRef(ctx->m, "image"))){
            return imageFunc(ctx, flags);
        }
    }
    return 0;
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
    ctx->stackIdx = ctx->it.p->nvalues;
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
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_TAG, tagFunc);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_BULLET, bulletFunc);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_TABLE, tableFunc);
        r = SUCCESS;
    }
    return r;
}
