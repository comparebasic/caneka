#include <external.h>
#include <caneka.h>

static char * cnkRangeToChars(word range){
    if(range == 0){
        return "ZERO";
    }else if(range == CNK_LANG_START){
        return "CNK_LANG_START";
    }else if(range == CNK_LANG_LINE){
        return "CNK_LANG_LINE";
    }else if(range == CNK_LANG_INDENT){
        return "CNK_LANG_INDENT";
    }else if(range == CNK_LANG_LINE_END){
        return "CNK_LANG_LINE_END";
    }else if(range == CNK_LANG_STRUCT){
        return "CNK_LANG_STRUCT";
    }else if(range == CNK_LANG_REQUIRE){
        return "CNK_LANG_REQUIRE";
    }else if(range == CNK_LANG_PACKAGE){
        return "CNK_LANG_PACKAGE";
    }else if(range == CNK_LANG_TYPE){
        return "CNK_LANG_TYPE";
    }else if(range == CNK_LANG_C){
        return "CNK_LANG_C";
    }else if(range == CNK_LANG_END_C){
        return "CNK_LANG_END_C";
    }else if(range == CNK_LANG_TOKEN){
        return "CNK_LANG_TOKEN";
    }else if(range == CNK_LANG_TOKEN_NULLABLE){
        return "CNK_LANG_TOKEN_NULLABLE";
    }else if(range == CNK_LANG_INVOKE){
        return "CNK_LANG_INVOKE";
    }else if(range == CNK_LANG_TOKEN_DOT){
        return "CNK_LANG_TOKEN_DOT";
    }else if(range == CNK_LANG_POST_TOKEN){
        return "CNK_LANG_POST_TOKEN";
    }else if(range == CNK_LANG_ASSIGN){
        return "CNK_LANG_ASSIGN";
    }else if(range == CNK_LANG_VALUE){
        return "CNK_LANG_VALUE";
    }else if(range == CNK_LANG_LINE_END){
        return "CNK_LANG_LINE_END";
    }else if(range == CNK_LANG_ARG_LIST){
        return "CNK_LANG_ARG_LIST";
    }else if(range == CNK_LANG_CURLY_OPEN){
        return "CNK_LANG_CURLY_OPEN";
    }else if(range == CNK_LANG_CURLY_CLOSE){
        return "CNK_LANG_CURLY_CLOSE";
    }else if(range == CNK_LANG_LIST_CLOSE){
        return "CNK_LANG_LIST_CLOSE";
    }else if(range == CNK_LANG_ROEBLING){
        return "CNK_LANG_ROEBLING";
    }else{
        return "unknown";
    }
}

static word indentDef[] = {
    PAT_MANY, ' ',' ', PAT_MANY|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word sepDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE, '\n', '\n', PAT_TERM, ',', ',',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word curlyCloseDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE, '\n', '\n', PAT_TERM, '}', '}',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word curlyOpenDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE, '\n', '\n', PAT_TERM, '{', '{',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word listCloseDef[] = {
    PAT_TERM, ')',')',
    PAT_END, 0, 0
};


static word lineEndDef[] = {
    PAT_ANY, ' ',' ', PAT_ANY,'\t','\t', PAT_ANY, '\r', '\r', PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};


static word reqDef[] = {
    PAT_TERM, 'R', 'R',
    PAT_TERM, 'e', 'e',
    PAT_TERM, 'q', 'q',
    PAT_TERM, 'u', 'u',
    PAT_TERM, 'i', 'i',
    PAT_TERM, 'r', 'r',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word pkgDef[] = {
    PAT_TERM, 'P', 'P',
    PAT_TERM, 'a', 'a',
    PAT_TERM, 'c', 'c',
    PAT_TERM, 'k', 'k',
    PAT_TERM, 'a', 'a',
    PAT_TERM, 'g', 'g',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word structDef[] = {
    PAT_TERM, 's', 's',
    PAT_TERM, 't', 't',
    PAT_TERM, 'r', 'r',
    PAT_TERM, 'u', 'u',
    PAT_TERM, 'c', 'c',
    PAT_TERM, 't', 't',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_TERM|PAT_NO_CAPTURE, '{', '{',
    PAT_END, 0, 0
};

static word typeDef[] = {
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'y', 'y',
    PAT_TERM, 'p', 'p',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE,'\t','\t', PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\r', '\r',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word rblDef[] = {
    PAT_TERM, '/', '/',
    PAT_END, 0, 0
};


static word tokenDef[] = {
    PAT_MANY|PAT_KO|PAT_LEAVE, '?', '?',PAT_MANY|PAT_KO|PAT_LEAVE, '.', '.', PAT_MANY|PAT_KO|PAT_LEAVE, '(', '(', PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', 
    PAT_END, 0, 0
};

static word invokeDef[] = {
    PAT_KO, '(', '(', PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z',
/*
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', PAT_TERM|PAT_NO_CAPTURE, '(', PAT_KO|PAT_MANY, ')', PAT_MANY|PAT_TERM, 0, 255,
    */
    PAT_END, 0, 0
};


static word tokenDotDef[] = {
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', PAT_TERM|PAT_NO_CAPTURE, '.','.',
    PAT_END, 0, 0
};

static word tokenNullDef[] = {
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', PAT_TERM|PAT_NO_CAPTURE, '?','?',
    PAT_END, 0, 0
};

static word cDef[] = {
    PAT_TERM, 'C', 'C',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_TERM, '{', '{',
    PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_NO_CAPTURE|PAT_TERM,'\n', '\n',
    PAT_END, 0, 0
};

static word lineDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static word assignDef[] = {
    PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_TERM, '=', '=', 
    PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_END, 0, 0
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)reqDef, CNK_LANG_REQUIRE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)pkgDef, CNK_LANG_PACKAGE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)typeDef, CNK_LANG_TYPE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cDef, CNK_LANG_C, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)structDef, CNK_LANG_STRUCT, CNK_LANG_STRUCT);

    PatCharDef *def = (PatCharDef *)lineDef;
    Match *mt = Span_ReserveNext(rbl->matches);
    r |= Match_SetPattern(mt, def);
    mt->captureKey = CNK_LANG_LINE;
    mt->jump = Roebling_GetMarkIdx(rbl, CNK_LANG_START);
    mt->type.state |= SUCCESS_EMPTY;

    return r;
}

static status str(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, CNK_LANG_STRUCT);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)indentDef, CNK_LANG_INDENT, CNK_LANG_STRUCT);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, CNK_LANG_POST_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNullDef, CNK_LANG_TOKEN_NULLABLE, CNK_LANG_POST_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_STRUCT);

    return r;
}

static status post(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)assignDef, CNK_LANG_ASSIGN, CNK_LANG_VALUE);

    return r;
}

static status value(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyOpenDef, CNK_LANG_CURLY_OPEN, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyCloseDef, CNK_LANG_CURLY_CLOSE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_VALUE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)invokeDef, CNK_LANG_INVOKE, CNK_LANG_ARG_LIST);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)rblDef, CNK_LANG_ROEBLING, -1);

    return r;
}

static status argList(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)listCloseDef, CNK_LANG_LIST_CLOSE, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)sepDef, 0, CNK_LANG_ARG_LIST);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, CNK_LANG_ARG_LIST);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_ARG_LIST);

    return r;
}

static status lineEnd(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, -1);
    return r;
}

static void pushItem(FmtCtx *ctx, word captureKey){
    FmtItem *item =  FmtItem_Make(ctx->m, ctx);
    item->spaceIdx = captureKey;
    item->parent = ctx->item;

    printf("setting iten as %s in %s\n", cnkRangeToChars(captureKey), cnkRangeToChars(ctx->item->spaceIdx)); 
    if(ctx->item->value == NULL){
        ctx->item->value = (Abstract *)Span_Make(ctx->m, TYPE_SPAN);
    }
    Span_Set((Span *)ctx->item->value, captureKey, (Abstract *)item);

    ctx->item = item;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_FMT_CTX);

    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture %s: %s ", DEBUG_LANG_CNK, 
            cnkRangeToChars(ctx->item->spaceIdx), cnkRangeToChars(captureKey));
        Debug_Print((void *)s, 0, " - ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }
    if(captureKey == CNK_LANG_TOKEN){
        if(ctx->item->spaceIdx == CNK_LANG_PACKAGE || ctx->item->spaceIdx == CNK_LANG_REQUIRE){
            ctx->item->spaceIdx = 0;
            Roebling_JumpTo(ctx->rbl, CNK_LANG_START);
        }
    }
    if(captureKey == CNK_LANG_INVOKE){
        pushItem(ctx, CNK_LANG_ARG_LIST);
    }
    if(captureKey > _CNK_LANG_MAJOR && captureKey < _CNK_LANG_MAJOR_END){
        pushItem(ctx, captureKey);
    }
    if(captureKey > _CNK_LANG_CLOSER && captureKey < _CNK_LANG_CLOSER_END){
        if(ctx->item->parent != NULL){
            ctx->item = ctx->item->parent;
            if(ctx->item->spaceIdx > 0){
                Roebling_JumpTo(ctx->rbl, ctx->item->spaceIdx);
            }
        }
        if(captureKey == CNK_LANG_CURLY_CLOSE){
            /* closes twice intentionally */
            if(ctx->item->parent != NULL){
                ctx->item = ctx->item->parent;
            }
        }
    }
    if(captureKey > _CNK_LANG_RETURNS && captureKey < _CNK_LANG_RETURNS_END){
        if(ctx->item->spaceIdx > 0){
            Roebling_JumpTo(ctx->rbl, ctx->item->spaceIdx);
        }
    }

    if(captureKey == CNK_LANG_INVOKE || captureKey == CNK_LANG_TOKEN_NULLABLE || captureKey == CNK_LANG_TOKEN || captureKey == CNK_LANG_VALUE){
        if(ctx->item->value == NULL){
            ctx->item->value = (Abstract *)Span_Make(ctx->m, TYPE_SPAN);
        }
        if(ctx->item->value->type.of == TYPE_SPAN){
            Span_Add((Span *)ctx->item->value, (Abstract *)s);
        }
    }

    return SUCCESS;
}

FmtCtx *CnkLangCtx_Make(MemCtx *m){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;


    Span *parsers = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_START));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_STRUCT));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)str));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_VALUE));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)value));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)lineEnd));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_POST_TOKEN));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)post));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_ARG_LIST));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)argList));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_LINE_END));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)lineEnd));

    LookupConfig config[] = {
        {CNK_LANG_START, (Abstract *)String_Make(m, bytes("START"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    ctx->rbl = Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        Capture,
        (Abstract *)ctx
    ); 
    ctx->rangeToChars = cnkRangeToChars;
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->spaceIdx = CNK_LANG_START;

    return ctx;
}
