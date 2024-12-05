#include <external.h>
#include <caneka.h>

struct pat_config {
    word flags;
    char *name; 
    int parent;
};

static struct pat_config pats[] = {
    {0, "p"},
    {0, "h1"},
    {0, "h2"},
    {0, "h3"},
    {0, "h4"},
    {0, "ul"},
    {FMT_FL_TO_PARENT, "li"},
    {FMT_FL_TO_NEXT_ID, "table"},
    {FMT_FL_TO_NEXT_ID|FMT_FL_TO_PARENT_ON_PARENT, "thead", -1},
    {FMT_FL_TO_NEXT_ID|FMT_FL_TO_PARENT_ON_PARENT, "tr", -1},
    {FMT_FL_TO_PARENT, "th"},
    {FMT_FL_TO_PARENT_ON_PARENT, "tr", -4},
    {FMT_FL_TO_PARENT, "td", -1},
    {0, NULL, 0},
};

static char *captureToChars(int captureKey){
   if(captureKey == FORMATTER_INDENT){
        return "FORMATTER_INDENT";
   }else if(captureKey == FORMATTER_ITEM){
        return "FORMATTER_ITEM";
   }else if(captureKey == FORMATTER_TABLE){
        return "FORMATTER_TABLE";
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
    PAT_ANY, ' ', ' ', PAT_TERM, '-', '-',
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

static status start(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)nlDef, FORMATTER_NEXT, FORMATTER_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)indentDef, FORMATTER_INDENT, FORMATTER_MARK_LINE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)dashDef, FORMATTER_ITEM, FORMATTER_MARK_LINE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)plusDef, FORMATTER_TABLE, FORMATTER_MARK_LINE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cmdDef, FORMATTER_METHOD, FORMATTER_MARK_VALUES);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineDef, FORMATTER_LINE, FORMATTER_MARK_START);

    return r;
}

static status line(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineDef, FORMATTER_LINE, FORMATTER_MARK_START);

    return r;
}

static status value(Roebling *rbl){
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
    ctx->offset = 0;
}

static void out(FmtCtx *ctx, int captureKey){
    FmtDef *def = Chain_Get(ctx->byId, ctx->id);
    if(def != NULL){
        printf("<%s>", def->name->bytes);
    }

    String *s = ctx->item->content;
    while(s != NULL){
        printf("%s", s->bytes);
        s = String_Next(s);
    }

    if(def != NULL){
        printf("</%s>", def->name->bytes);
    }

    if(captureKey == FORMATTER_LAST_VALUE){
        if(def->type.state & FMT_FL_TO_NEXT_ID){
            ctx->id++;
        }
    }else if(captureKey == FORMATTER_NEXT){
        ctx->id = 0;
    }

    reset(ctx);
}

static cls Fmt_MethodLookup(FmtCtx *ctx, String *s){
    return 0;
}

static status Fmt_SetMethod(FmtCtx *ctx, cls id){
    return NOOP;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    FmtCtx *ctx = as(source, TYPE_FMT_HTML);
    printf("\x1b[%dmcaptured %s/'%s'\x1b[0m\n", COLOR_YELLOW, captureToChars(captureKey), s->bytes);
    if(captureKey == FORMATTER_INDENT){
        ctx->id = s->length;
    }else if(captureKey == FORMATTER_LINE){
        String_Add(ctx->m, ctx->item->content, s);
    }else if(captureKey == FORMATTER_TABLE){
        FmtDef *def = TableChain_Get(ctx->byName, String_Make(ctx->m, bytes("table"))); 
        if(def != 0){
            ctx->id = def->id;
        }
    }else if(captureKey == FORMATTER_ITEM){
        FmtDef *def = TableChain_Get(ctx->byName, String_Make(ctx->m, bytes("ul"))); 
        if(def != 0){
            ctx->id = def->id;
        }
    }else if(captureKey == FORMATTER_METHOD){
        FmtDef *def = TableChain_Get(ctx->byName, s); 
        if(def != 0){
            ctx->id = def->id;
        }
    }else if(captureKey > _FORMATTER_OUT && captureKey < _FORMATTER_OUT_END){
        if(captureKey == FORMATTER_LINE || captureKey == FORMATTER_VALUE || captureKey == FORMATTER_LAST_VALUE){
            String_Add(ctx->m, ctx->item->content, s);
        }
        out(ctx, captureKey);
        if(captureKey == FORMATTER_NEXT){
            ctx->id = ZERO;
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
        def->id = lk->values->max_idx+1;
        Lookup_Add(fmt->m, lk, def->id, def);
        cnf++;
    }
    Fmt_Add(fmt->m, fmt, lk);
}

FmtCtx *FmtHtml_Make(MemCtx *m){
    FmtCtx *ctx = FmtCtx_Make(m);
    ctx->type.of = TYPE_FMT_HTML;
    ctx->type.state |= FMT_FL_AUTO_ID;

    addPatterns(ctx);

    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do =  Span_From(m, 7,
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_LINE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)line),
        (Abstract *)Int_Wrapped(m, FORMATTER_MARK_VALUES), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)value),
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
