#include <external.h>
#include <caneka.h>

typedef struct cnf  {
    word id;
    word state;
    int flags;
    char *name;
    char *alias;
    FmtTrans to;
    FmtTrans from;
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
            NULL,
            -1
        },
        {
            CNK_LANG_STRUCT,
            FMT_DEF_INDENT,
            0,
            "struct",
            NULL,
            CnkLang_StructTo,
            CnkLang_StructFrom,
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
            NULL,
            -1
        },
        {
            CNK_LANG_TOKEN,
            FMT_DEF_INDENT|FMT_DEF_PARENT_ON_PARENT,
            0,
            "token",
            NULL,
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
            NULL,
            -1
        },
        {
            CNK_LANG_LIST_CLOSE,
            FMT_DEF_OUTDENT,
            0,
            "list-close",
            NULL,
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
        def->from = cnf->from;
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

static void pushItem(FmtCtx *ctx, word captureKey, FmtDef *def){
    FmtItem *item =  FmtItem_Make(ctx->m, ctx);
    item->spaceIdx = captureKey;
    item->parent = ctx->item;
    item->def = def;

    ctx->item = item;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_FMT_CTX);

    FmtDef *def = Chain_Get(ctx->byId, captureKey);
    if(def == NULL || (def->type.state & FMT_FL_TAXONAMY) != 0){
        def = TableChain_Get(ctx->byAlias, s);
    }

    if(captureKey == CNK_LANG_INVOKE){
        if(ctx->item != NULL && ctx->item->parent != NULL){
            if(ctx->item->children == NULL){
                ctx->item->children = Span_Make(ctx->m, TYPE_SPAN);
            }
            Span_Add(ctx->item->parent->children, (Abstract *)ctx->item);
        }

        def = Chain_Get(ctx->byId, CNK_LANG_ARG_LIST);
    }

    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture %s:", DEBUG_LANG_CNK, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " - ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
        Debug_Print((void *)def, 0, "def: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }

    if(ctx->item != NULL && ctx->item->parent != NULL){
        if(def != NULL && ((def->type.state & FMT_DEF_OUTDENT) != 0)){
            if(ctx->item->def->from != NULL){
                ctx->item->value = ctx->item->def->from(ctx->m, 
                    ctx->item->def, ctx, ctx->item->key, (Abstract *)ctx->item);
            }
            if(ctx->item->def->to != NULL){
                ctx->item->def->to(ctx->m, 
                    ctx->item->def, ctx, ctx->item->key, (Abstract *)ctx->item);
            }
            FmtItem *item = ctx->item;
            while(item->parent != NULL && (item->def->type.state & FMT_DEF_PARENT_ON_PARENT) != 0){
                item = item->parent;
            }
            ctx->item = item;
        }
    }

    if(def != NULL){
        if(ctx->item->children == NULL){
            ctx->item->children = Span_Make(ctx->m, TYPE_SPAN);
        }
        printf("Adding %s to parent %s\n", CnkLang_RangeToChars(captureKey), CnkLang_RangeToChars(ctx->item->spaceIdx));
        Span_Add(ctx->item->children, (Abstract *)Result_Make(ctx->m, captureKey, s, source));
        
        if((def->type.state & FMT_DEF_INDENT) != 0){
            printf("indenting\n");
            pushItem(ctx, captureKey, def);
        }
    }

    if(DEBUG_LANG_CNK){
        Debug_Print((void *)ctx->item->def, 0, "Base FmtDef: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
        printf("\n");
    }

    if(captureKey == CNK_LANG_LINE_END || captureKey == CNK_LANG_CURLY_CLOSE){
        Roebling_JumpTo(ctx->rbl, ctx->item->spaceIdx);
    }

    return SUCCESS;
}

FmtCtx *CnkLangCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;
    ctx->rbl = CnkLangCtx_RblMake(m, ctx, Capture);
    ctx->out = ToStdOut;

    ctx->rangeToChars = CnkLang_RangeToChars;
    addDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->item->spaceIdx = CNK_LANG_START;
    FmtDef *def = Chain_Get(ctx->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    ctx->source = source;

    return ctx;
}
