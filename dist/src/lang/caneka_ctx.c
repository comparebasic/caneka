#include <external.h>
#include <caneka.h>

typedef struct cnf  {
    word id;
    word state;
    int flags;
    char *name;
    char *alias;
    FmtTrans to;
    word *rollups;
    int parentIdx;
} Cnf;

word rollupToken[] = {CNK_LANG_TOKEN_DOT,
    CNK_LANG_TOKEN_NULLABLE, CNK_LANG_INVOKE, 0};
word rollupOp[] = {CNK_LANG_ASSIGN,CNK_LANG_ADD_AND,CNK_LANG_GT,CNK_LANG_LT,
    CNK_LANG_IS,CNK_LANG_NOT,CNK_LANG_EQ,CNK_LANG_CMP,CNK_LANG_FLAG_ADD,
    CNK_LANG_FLAG_SUB,0};

static status addDefs(FmtCtx *ctx){
    status r = READY;
    MemCtx *m = ctx->m;

    Cnf configs[] = {
        {
            CNK_LANG_START,
            0,
            0,
            "struct",
            NULL,
            CnkLang_Start,
            NULL,
            -1
        },
        {
            CNK_LANG_STRUCT,
            FMT_DEF_INDENT,
            0,
            "struct",
            NULL,
            CnkLang_Struct,
            NULL,
            -1
        },
        {
            CNK_LANG_REQUIRE,
            FMT_DEF_INDENT,
            0,
            "require",
            NULL,
            CnkLang_RequireTo,
            NULL,
            -1
        },
        {
            CNK_LANG_PACKAGE,
            FMT_DEF_INDENT,
            0,
            "package",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_TYPE,
            FMT_DEF_INDENT,
            0,
            "type",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_ARG_LIST,
            FMT_DEF_INDENT,
            0,
            "arg-list",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_C,
            FMT_DEF_INDENT,
            0,
            "c",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_TOKEN,
            0,
            0,
            "token",
            NULL,
            NULL,
            rollupToken,
            -1
        },
        {
            CNK_LANG_OP,
            0,
            0,
            "op",
            NULL,
            NULL,
            rollupOp,
            -1
        },
        {
            CNK_LANG_VALUE,
            0,
            0,
            "value",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_ROEBLING,
            0,
            0,
            "roebling",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_KEYS,
            0,
            0,
            "keys",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_IDXS,
            0,
            0,
            "idxs",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_SEQ,
            0,
            0,
            "seq",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_LIST_CLOSE,
            FMT_DEF_OUTDENT,
            0,
            "seq",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_CURLY_CLOSE,
            FMT_DEF_OUTDENT,
            0,
            NULL,
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            0,
            0,
            0,
            NULL,
            NULL,
            NULL,
            NULL,
            -1
        },
    };

    Lookup *lk = Lookup_Make(ctx->m, CNK_LANG_START, NULL, NULL);
    Cnf *cnf = configs;
    while(cnf->id > 0){
        
        FmtDef *def = FmtDef_Make(m);
        def->id = cnf->id;
        def->type.state = cnf->state;
        def->flags = cnf->flags;
        if(cnf->name != NULL){
            def->name = String_Make(m, bytes(cnf->name));
        }
        if(cnf->alias != NULL){
            def->alias = String_Make(m, bytes(cnf->alias));
        }
        def->to = cnf->to;
        if(def->parent > 0){
            def->parent = Lookup_Get(lk, cnf->parentIdx);
        }

        if(cnf->rollups != 0){
            def->type.state |= FMT_FL_TAXONAMY;
            word *rl = cnf->rollups;
            while(*rl != 0){
                r |= Lookup_Add(ctx->m, lk, *rl, def);
                rl++;
            }
        }
        r |= Lookup_Add(ctx->m, lk, cnf->id, def);
        cnf++;
    }

    return Fmt_Add(ctx->m, ctx, lk);
}

static void pushItem(FmtCtx *ctx, word captureKey){
    FmtItem *item =  FmtItem_Make(ctx->m, ctx);
    item->spaceIdx = captureKey;
    item->parent = ctx->item;

    if(item->parent->children == NULL){
        item->parent->children = Span_Make(ctx->m, TYPE_SPAN);
    }
    Span_Add(item->parent->children, (Abstract *)item);

    ctx->item = item;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_FMT_CTX);

    FmtDef *def = Chain_Get(ctx->byId, captureKey);
    if(def == NULL || (def->type.state & FMT_FL_TAXONAMY) != 0){
        TableChain_Get(ctx->byAlias, s);
    }

    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture %s:", DEBUG_LANG_CNK, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " - ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
        Debug_Print((void *)def, 0, "FmtDef: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }

    if(def != NULL){
        if((def->type.state & FMT_DEF_INDENT) != 0){
            pushItem(ctx, captureKey);
        }else if((def->type.state & FMT_DEF_OUTDENT) != 0 && ctx->item != NULL){
            ctx->item = ctx->item->parent;
        }

        if(ctx->item != NULL && ctx->item->def == NULL){
            ctx->item->def = def;
        }
    }
    printf("\n");

    /*
    if(captureKey == CNK_LANG_TOKEN){
        if(ctx->item->spaceIdx == CNK_LANG_PACKAGE || ctx->item->spaceIdx == CNK_LANG_REQUIRE){
            ctx->item->spaceIdx = 0;
            Roebling_JumpTo(ctx->rbl, CNK_LANG_START);
        }
    }
    if(captureKey == CNK_LANG_OP){
        printf("Getting from table\n");
        FmtDef *def = TableChain_Get(ctx->byAlias, s);
        / * set operation by alias * /
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
            / * closes twice intentionally * /
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

    if(captureKey == CNK_LANG_INVOKE || captureKey == CNK_LANG_TOKEN_NULLABLE || captureKey == CNK_LANG_TOKEN || captureKey == CNK_LANG_VALUE || captureKey == CNK_LANG_TOKEN_DOT){
        if(ctx->item->value == NULL){
            ctx->item->value = (Abstract *)Span_Make(ctx->m, TYPE_SPAN);
        }
        if(ctx->item->value->type.of == TYPE_SPAN){
            Span_Add((Span *)ctx->item->value, (Abstract *)s);
        }
    }
    */

    return SUCCESS;
}

static status genOut(FmtCtx *ctx, FmtItem *item, OutFunc out){
    String *s = (String *)item->def->to(ctx->m, item->def, ctx, item->key, (Abstract *)item);
    return out(ctx->m, s, ctx->source);
}

static status Generate(FmtCtx *ctx, OutFunc out){
    FmtItem *item = ctx->root;

    String *ext = String_Make(ctx->m, bytes(".cnk"));

    Transp *tp = (Transp *)as(ctx->source, TYPE_TRANSP);
    String *module = String_ToCamel(ctx->m, tp->current.fname);
    String_Trunc(module, ext->length);
    ctx->root->value = (Abstract *)module;

    Iter it;
    while(item != NULL){
        if(item->def->to != NULL){
            genOut(ctx, item, out);
        }


        if(item->children != NULL){
            Iter_Init(&it, item->children);
            if((Iter_Next(&it) & END) == 0){
                FmtItem *itm = (FmtItem *)Iter_Get(&it);
                if(itm != NULL && itm->def != NULL && itm->def->to != NULL){
                    genOut(ctx, itm, out);
                }
            }
        }
        item = NULL;
    }
    return NOOP;
}


FmtCtx *CnkLangCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;
    ctx->rbl = CnkLangCtx_RblMake(m, ctx, Capture);
    ctx->generate = Generate;

    ctx->rangeToChars = CnkLang_RangeToChars;
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->spaceIdx = CNK_LANG_START;
    addDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->item->spaceIdx = CNK_LANG_START;
    FmtDef *def = Chain_Get(ctx->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    ctx->source = source;

    return ctx;
}
