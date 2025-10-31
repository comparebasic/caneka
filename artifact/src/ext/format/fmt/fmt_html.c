#include <external.h>
#include <caneka.h>

static Lookup *fmtToHtmlLookup = NULL;

static i64 tableFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = Iter_Current(&ctx->it);
    Str *table = Str_CstrRef(ctx->m, "TABLE");
    if(flags & TRANSP_OPEN){
        total += Tag_Out(ctx->bf, (Abstract *)table, ZERO);
        total += Buff_Bytes(ctx->bf, (byte *)"\n", 1);
    }
    if(flags & TRANSP_BODY){
        Str *thead = Str_CstrRef(ctx->m, "THEAD");
        Str *tr = Str_CstrRef(ctx->m, "TR");
        Str *td = Str_CstrRef(ctx->m, "TD");
        Relation *rel = (Relation *)as(a, TYPE_RELATION);
        Relation_ResetIter(rel);

        Buff_Bytes(ctx->bf, (byte *)"  ", 2);
        if(rel->it.idx == 0){
            total += Tag_Out(ctx->bf, (Abstract *)thead, ZERO);
            total += Tag_Out(ctx->bf, (Abstract *)tr, ZERO);
        }
        Abstract **hdr = rel->headers; 
        for(i32 i = 0; i < rel->stride; i++, hdr++){
            total += Tag_Out(ctx->bf, (Abstract *)td, ZERO);
            total += ToS(ctx->bf, *hdr, 0, ZERO);
            total += Tag_Out(ctx->bf, (Abstract *)td, TAG_CLOSE);
        }
        total += Tag_Out(ctx->bf, (Abstract *)tr, TAG_CLOSE);
        total += Tag_Out(ctx->bf, (Abstract *)thead, TAG_CLOSE);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);

        while((Relation_Next(rel) & END) == 0){
            if(rel->type.state & RELATION_ROW_START){
                Buff_Bytes(ctx->bf, (byte *)"  ", 2);
                total += Tag_Out(ctx->bf, (Abstract *)tr, ZERO);
            }
            total += Tag_Out(ctx->bf, (Abstract *)td, ZERO);
            total += ToS(ctx->bf, Iter_Current(&rel->it), 0, ZERO);
            total += Tag_Out(ctx->bf, (Abstract *)td, TAG_CLOSE);
            if(rel->type.state & RELATION_ROW_END){
                total += Tag_Out(ctx->bf, (Abstract *)tr, TAG_CLOSE);
                total += Buff_Bytes(ctx->bf, (byte *)"\n", 1);
            }
        }
    }
    if(flags & TRANSP_CLOSE){
        total += Tag_Out(ctx->bf, (Abstract *)table, TAG_CLOSE);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);
    }
    return total;
}


static i64 bulletFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = Iter_Current(&ctx->it);
    if(flags & TRANSP_OPEN){
        Str *s = Str_CstrRef(ctx->m, "UL");
        total += Tag_Out(ctx->bf, (Abstract *)s, ZERO);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);
    }
    if(flags & TRANSP_BODY){
        Buff_Bytes(ctx->bf, (byte *)"  ", 2);
        Str *s = Str_CstrRef(ctx->m, "LI");
        total += Tag_Out(ctx->bf, (Abstract *)s, ZERO);
        total += ToS(ctx->bf, a, 0, ZERO);
        total += Tag_Out(ctx->bf, (Abstract *)s, TAG_CLOSE);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);
    }
    if(flags & TRANSP_CLOSE){
        Str *s = Str_CstrRef(ctx->m, "UL");
        total += Tag_Out(ctx->bf, (Abstract *)s, TAG_CLOSE);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);
    }
    return total;
}

static i64 headerFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = Iter_Current(&ctx->it);
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
        total += Tag_Out(ctx->bf, (Abstract *)s, ZERO);
    }
    if(flags & TRANSP_BODY){
        total += ToS(ctx->bf, a, 0, ZERO);
    }
    if(flags & TRANSP_CLOSE){
        total += Tag_Out(ctx->bf, (Abstract *)s, TAG_CLOSE);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);
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
    return Fmt(ctx->bf, "<A href=\"$\">$</A>", args);
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
    return Fmt(ctx->bf, "<IMG alt=\"$\" src=\"$\" />", args);
}

static i64 paragraphFunc(TranspCtx *ctx, word flags){
    i64 total = 0;
    Abstract *a = Iter_Current(&ctx->it);
    Str *s = Str_CstrRef(ctx->m, "P");
    if(flags & TRANSP_OPEN){
        total += Tag_Out(ctx->bf, (Abstract *)s, ZERO);
    }
    if(flags & TRANSP_BODY){
        total += ToS(ctx->bf, a, 0, ZERO);
    }
    if(flags & TRANSP_CLOSE){
        total += Tag_Out(ctx->bf, (Abstract *)s, TAG_CLOSE);
        Buff_Bytes(ctx->bf, (byte *)"\n", 1);
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

status Fmt_ToHtml(Buff *bf, Mess *mess){
    MemCh *m = bf->m;
    if(fmtToHtmlLookup == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "FmtToHtml no initialized", NULL);
        return ERROR;
    }

    if(mess->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)mess,
            NULL
        };
        Out("^b.Fmt_ToHtml(&)\n", args);
    }

    TranspCtx *ctx = TranspCtx_Make(m, bf, fmtToHtmlLookup);
    ctx->type.state |= (mess->type.state & DEBUG);
    mess->transp = ctx;

    Iter_SetByIdx(&ctx->it, 0, (Abstract *)mess->root);
    ctx->stackIdx = ctx->it.p->nvalues;
    i64 total = 0;
    while((ctx->type.state & (SUCCESS|ERROR|ERROR)) == 0){
        total += Transp(ctx);
    }

    if(ctx->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)ctx,
            NULL
        };
        Out("^y.Fmt_ToHtml(ctx:&)\n", args);
    }

    return total;
}

status FmtToHtml_Init(MemCh *m){
    status r = READY;
    if(fmtToHtmlLookup == NULL){
        fmtToHtmlLookup = Lookup_Make(m, _TYPE_ZERO);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_INDENT, headerFunc);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_PARAGRAPH, paragraphFunc);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_TAG, tagFunc);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_BULLET, bulletFunc);
        Lookup_Add(m, fmtToHtmlLookup, FORMATTER_TABLE, tableFunc);
        r = SUCCESS;
    }
    return r;
}
