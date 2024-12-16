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
    CNK_LANG_TOKEN_NULLABLE, 0};
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

        r |= Lookup_Add(ctx->m, lk, cnf->id, def);
        if(cnf->rollups != 0){
            word *rl = cnf->rollups;
            while(*rl != 0){
                r |= Lookup_Add(ctx->m, lk, *rl, def);
                rl++;
            }
        }
        cnf++;
    }

    return Fmt_Add(ctx->m, ctx, lk);
}


FmtCtx *CnkLangCtx_Make(MemCtx *m){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;
    ctx->rbl = CnkLangCtx_RblMake(m, ctx);

    ctx->rangeToChars = CnkLang_RangeToChars;
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->spaceIdx = CNK_LANG_START;
    addDefs(ctx);

    return ctx;
}
