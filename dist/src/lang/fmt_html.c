#include <external.h>
#include <caneka.h>

struct pat_config {
    word flags;
    char *alias;
    char *name; 
    int parent;
};

static struct pat_config pats[] = {
    {FMT_FL_SPACE_FOR_WS, NULL, "p", 0},
    {FMT_FL_SPACE_FOR_WS, NULL, "h1", 0},
    {FMT_FL_SPACE_FOR_WS, NULL, "h2", 0},
    {FMT_FL_SPACE_FOR_WS, NULL, "h3", 0},
    {FMT_FL_SPACE_FOR_WS, NULL, "h4", 0},
    {FMT_FL_SPACE_FOR_WS|FMT_FL_CLOSE_AFTER_CHILD|FMT_FL_TO_NEXT_ID, "-", "ul", 0},
    {FMT_FL_SPACE_FOR_WS|FMT_FL_TO_PARENT|FMT_FL_SPACE_FOR_WS, NULL, "li", -1},
    {FMT_FL_TO_NEXT_ID|FMT_FL_CLOSE_AFTER_CHILD, "+", "table", 0},
    {FMT_FL_TO_NEXT_ID|FMT_FL_CLOSE_AFTER_CHILD, NULL, "thead", -1},
    {FMT_FL_TO_NEXT_ID|FMT_FL_TO_PARENT_ON_LAST|FMT_FL_CLOSE_AFTER_CHILD|FMT_FL_TO_NEXT_ID, NULL, "tr", -1},
    {FMT_FL_TO_PARENT_ON_LAST|FMT_FL_TO_NEXT_ON_LAST , NULL, "th", -1},
    {FMT_FL_TO_PARENT|FMT_FL_TO_NEXT_ID|FMT_FL_CLOSE_AFTER_CHILD, NULL, "tr", -4},
    {FMT_FL_TO_PARENT_ON_LAST, NULL, "td", -1},
    {0, NULL, NULL, 0},
};

static char *captureToChars(int captureKey){
   if(captureKey == FORMATTER_INDENT){
        return "FORMATTER_INDENT";
   }else if(captureKey == FORMATTER_ALIAS){
        return "FORMATTER_ALIAS";
   }else if(captureKey == FORMATTER_LINE){
        return "FORMATTER_LINE";
   }else if(captureKey == FORMATTER_LAST_VALUE){
        return "FORMATTER_LAST_VALUE";
   }else if(captureKey == FORMATTER_VALUE){
        return "FORMATTER_VALUE";
   }else if(captureKey == FORMATTER_NEXT){
        return "FORMATTER_NEXT";
   }else if(captureKey == FORMATTER_METHOD){
        return "FORMATTER_METHOD";
   }else{
        return "unknown";
   }
}

static word indentDef[] = {
    PAT_ANY|PAT_NO_CAPTURE,' ' ,' ',PAT_MANY|PAT_TERM, '>', '>',PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,' ' ,' ',
    PAT_END, 0, 0
};

static word lineDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static word nlDef[] = {
    PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word dashDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ', ' ', PAT_TERM, '-', '-',PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,' ' ,' ',
    PAT_END, 0, 0
};

static word plusDef[] = {
    PAT_ANY, ' ', ' ', PAT_TERM, '+', '+',
    PAT_END, 0, 0
};


static word cmdDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '.', '.', PAT_KO, ':', ':', patText,
    PAT_END, 0, 0
};

static word commaSepDef[] = {
    PAT_KO, ',', ',', patText,
    PAT_END, 0, 0
};

static word lastValueDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)nlDef, FORMATTER_NEXT, FORMATTER_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)indentDef, FORMATTER_INDENT, FORMATTER_MARK_LINE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)dashDef, FORMATTER_ALIAS, FORMATTER_MARK_LINE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)plusDef, FORMATTER_ALIAS, FORMATTER_MARK_VALUES);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cmdDef, FORMATTER_METHOD, FORMATTER_MARK_VALUES);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineDef, FORMATTER_LINE, FORMATTER_MARK_START);

    return r;
}

static status line(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineDef, FORMATTER_LINE, FORMATTER_MARK_START);

    return r;
}

static status value(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)nlDef, FORMATTER_NEXT, FORMATTER_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)commaSepDef, FORMATTER_VALUE, FORMATTER_MARK_VALUES);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lastValueDef, FORMATTER_LAST_VALUE, FORMATTER_MARK_VALUES);

    return r;
}

static void reset(FmtCtx *ctx){
    String_Reset(ctx->item->content);
}

static void outClose(FmtCtx *ctx, word fl){
    FmtDef *curDef = ctx->def;
    while(curDef != NULL && curDef->parent != NULL &&  
           (curDef->type.state & fl) != 0){
       if(curDef->parent->type.state & FMT_FL_CLOSE_AFTER_CHILD){
            printf("</%s>", curDef->parent->name->bytes);
       }
       curDef = curDef->parent;
    }
}

static void out(FmtCtx *ctx, int captureKey){
    FmtDef *def = ctx->def;
    printf("<%s>", def->name->bytes);

    if((def->type.state & FMT_FL_TO_NEXT_ID) != 0){
        ctx->def = Chain_Get(ctx->byId, def->id+1); 
        out(ctx, captureKey);
        return;
    }

    String *s = ctx->item->content;
    while(s != NULL){
        printf("%s", s->bytes);
        s = String_Next(s);
    }

    if((def->type.state & FMT_FL_CLOSE_AFTER_CHILD) == 0){
        printf("</%s>", def->name->bytes);
    }

    if(captureKey == FORMATTER_LAST_VALUE){
        outClose(ctx, FMT_FL_TO_PARENT_ON_LAST);
        if(def->type.state & FMT_FL_TO_NEXT_ON_LAST){
            ctx->def = Chain_Get(ctx->byId, ctx->def->id+1);
        }else{
            ctx->def = Chain_Get(ctx->byId, ctx->def->id-1);
        }
    }else if(captureKey == FORMATTER_NEXT){
        outClose(ctx, (FMT_FL_TO_PARENT|FMT_FL_TO_PARENT_ON_LAST));
        ctx->def = NULL;
        reset(ctx);
    }
}

static cls Fmt_MethodLookup(FmtCtx *ctx, String *s){
    return 0;
}

static status Fmt_SetMethod(FmtCtx *ctx, cls id){
    return NOOP;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    FmtCtx *ctx = as(source, TYPE_FMT_HTML);
    /*
    printf("\x1b[%dmcaptured %s/'%s'\x1b[0m\n", COLOR_YELLOW, captureToChars(captureKey), s->bytes);
    */
    if(ctx->def == NULL){
        ctx->def = Chain_Get(ctx->byId, 0);
    }

    if(captureKey == FORMATTER_INDENT){
        ctx->def = Chain_Get(ctx->byId, s->length);
    }else if(captureKey == FORMATTER_ALIAS || captureKey == FORMATTER_METHOD){
        if(String_Length(ctx->item->content) > 0){
            out(ctx, captureKey);
            reset(ctx);
        }
        FmtDef *def = NULL;
        if(captureKey == FORMATTER_ALIAS){
            def = TableChain_Get(ctx->byAlias, s); 
        }else{
            def = TableChain_Get(ctx->byName, s); 
        }
        if(def != NULL){
            if(ctx->def == NULL || ctx->def->parent != def){
                ctx->def = def;
            }
        }
    }else if(captureKey > _FORMATTER_OUT && captureKey < _FORMATTER_OUT_END){
        if(captureKey == FORMATTER_LINE || captureKey == FORMATTER_VALUE || captureKey == FORMATTER_LAST_VALUE){
            if(ctx->def != NULL && (ctx->def->type.state & FMT_FL_SPACE_FOR_WS) != 0 && String_Length(ctx->item->content) > 0){
                String_AddBytes(ctx->m, ctx->item->content, bytes(" "), 1);
            }
            String_Add(ctx->m, ctx->item->content, s);
        }
        if(captureKey == FORMATTER_VALUE || captureKey == FORMATTER_LAST_VALUE){
            out(ctx, captureKey);
            reset(ctx);
        }
        if(captureKey == FORMATTER_NEXT){
            out(ctx, captureKey);
            reset(ctx);
            ctx->def = NULL;
        }

        if(captureKey == FORMATTER_NEXT || captureKey == FORMATTER_LAST_VALUE){
            printf("\n");
        }

    }
    return SUCCESS;
}

static void addPatterns(FmtCtx *fmt){
    struct pat_config *cnf = pats;
    Lookup *lk = Lookup_Make(fmt->m, 0, NULL, (Abstract *)fmt);
    while(cnf->name != NULL){
        FmtDef *def = FmtDef_Make(fmt->m);
        def->name = String_Make(fmt->m, bytes(cnf->name));
        if(cnf->alias != NULL){
            def->alias = String_Make(fmt->m, bytes(cnf->alias));
        }
        def->id = lk->values->max_idx+1;
        def->type.state = cnf->flags;
        if(cnf->parent != 0){
            def->parent = Lookup_Get(lk, def->id+cnf->parent);
        }
        Lookup_Add(fmt->m, lk, def->id, def);
        cnf++;
    }
    Fmt_Add(fmt->m, fmt, lk);
}

FmtCtx *FmtHtml_Make(MemCtx *m){
    FmtCtx *ctx = FmtCtx_Make(m, NULL);
    ctx->type.of = TYPE_FMT_HTML;
    ctx->type.state |= FMT_FL_AUTO_ID;

    addPatterns(ctx);

    Span *parsers_do =  Span_From(m, 7,
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_START), 
            (Abstract *)Do_Wrapped(m, (DoFunc)start),
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_LINE), 
            (Abstract *)Do_Wrapped(m, (DoFunc)line),
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_VALUES), 
            (Abstract *)Do_Wrapped(m, (DoFunc)value),
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_END));

    LookupConfig config[] = {
        {FORMATTER_MARK_START, (Abstract *)String_Make(m, bytes("START"))},
        {FORMATTER_MARK_LINE, (Abstract *)String_Make(m, bytes("LINE"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    ctx->rbl = Roebling_Make(m, TYPE_ROEBLING,
        parsers_do,
        desc,
        s,
        Capture,
        (Abstract *)ctx
    ); 
    ctx->item = FmtItem_Make(m, ctx);
    return ctx;
}
