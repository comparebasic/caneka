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
word rollupOp[] = {CNK_LANG_ADD_AND,CNK_LANG_GT,CNK_LANG_LT,
    CNK_LANG_IS,CNK_LANG_NOT,CNK_LANG_EQ,CNK_LANG_CMP,CNK_LANG_FLAG_ADD,
    CNK_LANG_FLAG_SUB,0};

status CnkLang_AddDefs(FmtCtx *ctx){
    status r = READY;
    MemCtx *m = ctx->m;

    Cnf configs[] = {
        {
            CNK_LANG_START,
            0,
            0,
            "struct",
            NULL,
            CnkLang_StartTo,
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
            FMT_DEF_INDENT|FMT_DEF_PARENT_ON_PARENT,
            0,
            "token",
            NULL,
            NULL,
            rollupToken,
            -1
        },
        {
            CNK_LANG_ASSIGN,
            0,
            0,
            "op",
            "=",
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
            "list-close",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_CURLY_CLOSE,
            FMT_DEF_OUTDENT,
            0,
            "curly-close",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_BLANK_LINE,
            0,
            0,
            NULL,
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_LINE_END,
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

