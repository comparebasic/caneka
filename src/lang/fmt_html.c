#include <external.h>
#include <caneka.h>

static char *captureToChars(int captureKey){
   if(captureKey == FORMATTER_INDENT){
        return "FORMATTER_INDENT";
   }else if(captureKey == FORMATTER_ITEM){
        return "FORMATTER_ITEM";
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
    if(captureKey == FORMATTER_NEXT){
        if(ctx->id == FMT_TBL){
            printf("</table>");
            return;
        }
    }

    if(ctx->id != 0){
        if(ctx->id == FMT_UL){
            if(ctx->offset == 0){
                printf("<ul>");
            }else{
                printf("<li>");
            }
        }else if(ctx->id == FMT_TBL){
            if(ctx->offset == 0){
                printf("<table><thead><tr>");
            }else if(ctx->offset == 1){
                printf("<th>");
            }else{
                printf("<td>");
            }
        }
    }else{
        if(ctx->offset == 1){
            printf("<h1>");
        }else if(ctx->offset == 2){
            printf("<h2>");
        }else if(ctx->offset == 3){
            printf("<h3>");
        }else if(ctx->offset == 3){
            printf("<h4>");
        }else{
            printf("<p>");
        }
    }

    String *s = ctx->item->content;
    while(s != NULL){
        printf("%s", s->bytes);
        s = String_Next(s);
    }

    if(captureKey == FORMATTER_LAST_VALUE){
        if(ctx->id == FMT_TBL){
            if(ctx->offset == 1){
                printf("</tr></thead>\n");
            }else{
                printf("</td></tr>\n");
            }
        }
    }else if(captureKey == FORMATTER_NEXT){
        if(ctx->id == FMT_TBL){
            printf("</table>\n");
        }else if(ctx->id == FMT_UL){
            printf("</ul>\n");
        }
    }else{
        if(ctx->id != 0){
            if(ctx->id == FMT_UL){
                if(ctx->offset == 0){
                    printf("</ul>");
                }else{
                    printf("</li>");
                }
            }else if(ctx->id == FMT_TBL){
                if(ctx->offset == 1){
                    printf("</th>");
                }else{
                    printf("</td>");
                }
            }
        }else{
            if(ctx->offset == 1){
                printf("</h1>\n");
            }else if(ctx->offset == 2){
                printf("</h2>\n");
            }else if(ctx->offset == 3){
                printf("</h3>\n");
            }else if(ctx->offset == 3){
                printf("</h4>\n");
            }else{
                printf("</p>\n");
            }
        }
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
        ctx->offset = s->length;
    }else if(captureKey == FORMATTER_LINE){
        String_Add(ctx->m, ctx->item->content, s);
    }else if(captureKey == FORMATTER_METHOD){
        if(String_EqualsBytes(s, bytes("tbl"))){
            printf("setting table\n");
            ctx->id = FMT_TBL;
        }
    }else if(captureKey > _FORMATTER_OUT && captureKey < _FORMATTER_OUT_END){
        if(captureKey == FORMATTER_VALUE || captureKey == FORMATTER_LAST_VALUE){
            if(ctx->id == FMT_TBL){
                out(ctx, captureKey);
                if(ctx->offset == 0){
                    ctx->offset++;
                }
            }else if(ctx->id == FMT_UL){
                out(ctx, captureKey);
                if(ctx->offset == 0){
                    ctx->offset++;
                }
            }

            String_Add(ctx->m, ctx->item->content, s);
            out(ctx, captureKey);

            if(captureKey == FORMATTER_LAST_VALUE){
                if(ctx->offset == 1){
                    ctx->offset++;
                }
            }
        }else{
            out(ctx, captureKey);
        }
        if(captureKey == FORMATTER_NEXT){
            ctx->id = ZERO;
        }
    }
    return SUCCESS;
}



FmtCtx *FmtHtml_Make(MemCtx *m){
    FmtCtx *ctx = FmtCtx_Make(m);
    ctx->type.of = TYPE_FMT_HTML;

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
