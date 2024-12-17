#include <external.h>
#include <caneka.h>

typedef struct cnf  {
    word id;
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
            CNK_LANG_STRUCT,
            "struct",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_REQUIRE,
            "require",
            NULL,
            CnkLang_RequireTo,
            NULL,
            -1
        },
        {
            CNK_LANG_PACKAGE,
            "package",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_TYPE,
            "type",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_ARG_LIST,
            "arg-list",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_C,
            "c",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_TOKEN,
            "token",
            NULL,
            NULL,
            rollupToken,
            -1
        },
        {
            CNK_LANG_OP,
            "op",
            NULL,
            NULL,
            rollupOp,
            -1
        },
        {
            CNK_LANG_VALUE,
            "value",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_ROEBLING,
            "roebling",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_KEYS,
            "keys",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_IDXS,
            "idxs",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_SEQ,
            "seq",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
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

    printf("setting iten as %s in %s\n", CnkLang_RangeToChars(captureKey), CnkLang_RangeToChars(ctx->item->spaceIdx)); 
    if(ctx->item->value == NULL){
        ctx->item->value = (Abstract *)Span_Make(ctx->m, TYPE_SPAN);
    }
    Span_Set((Span *)ctx->item->value, captureKey, (Abstract *)item);

    ctx->item = item;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_FMT_CTX);

    FmtDef *def = Chain_Get(ctx->ById, captureKey);
    if(def == NULL || (def->tpye.state & FMT_FL_TAXONAMY) != 0){
        TableChain_Get(ctx->byAlias, s);
    }

    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture %s: %s ", DEBUG_LANG_CNK, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " - ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
        Debug_Print((void *)def, 0, "FmtDef: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }

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

FmtCtx *CnkLangCtx_Make(MemCtx *m){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;
    ctx->rbl = CnkLangCtx_RblMake(m, ctx, Capture);

    ctx->rangeToChars = CnkLang_RangeToChars;
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->spaceIdx = CNK_LANG_START;
    addDefs(ctx);

    return ctx;
}
