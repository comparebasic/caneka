#include <external.h>
#include <caneka.h>

Abstract *CnkRbl_Pat(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a){
    Debug_Print((void *)s, 0, "Found Pat: ", COLOR_PURPLE, TRUE);
    printf("\n");

    FmtItem *item = fmt->item;
    if(item == NULL){
        Fatal("Need item to add pattern to", TYPE_LANG_CNK_RBL);
        return NULL;
    }

    IterStr it;
    byte out;
    IterStr_Init(&it, s, sizeof(byte), bytes(&out));
    
    PatCharDef pat;
    while((IterStr_Next(&it) & END) == 0){
        byte *b = (byte *)IterStr_Get(&it);
        pat.flags = PAT_TERM;
        pat.from = *b;
        pat.to = *b;
        String_AddBytes(m, item->content, bytes(&pat), sizeof(PatCharDef));
    }

    return NULL;
}

Abstract *CnkRbl_PatClose(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a){
    Debug_Print((void *)s, 0, "Found Pat: ", COLOR_PURPLE, TRUE);
    printf("\n");

    FmtItem *item = fmt->item;
    if(item == NULL){
        Fatal("Need item to add pattern to", TYPE_LANG_CNK_RBL);
        return NULL;
    }

    PatCharDef pat;
    pat.flags = PAT_END;
    pat.from = 0;
    pat.to = 0;
    String_AddBytes(m, item->content, bytes(&pat), sizeof(PatCharDef));

    Debug_Print((void *)item->content->bytes, TYPE_PATCHARDEF, "Defined Pat: ", COLOR_PURPLE, TRUE);
    printf("\n");

    return NULL;
}

Abstract *CnkRbl_PatKeyOpen(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a){
    String_AddBytes(m, s, bytes("pat"), 3);
    String_AddInt(m, s, ctx->root->value->nvalues);
    Debug_Print((void *)s, 0, "Making Pat Table Entry: ", COLOR_PURPLE, TRUE);
    printf("\n");

    return NULL;
}

status CnkRblLang_AddDefs(FmtCtx *ctx){
    status r = READY;
    MemCtx *m = ctx->m;

    FmtCnf configs[] = {
        {
            CNK_LANG_RBL_OPEN_CLOSE,
            0,
            0,
            "",
            NULL,
            NULL,
            NULL,
            -1
        },
        {
            CNK_LANG_RBL_PAT_KEY,
            0,
            FMT_DEF_INDENT,
            "",
            NULL,
            CnkRbl_PatKeyOpen,  
            NULL,
            -1
        },
        {
            CNK_LANG_RBL_PAT_KEY_CLOSE,
            0,
            FMT_FL_TO_PARENT,
            "",
            NULL,
            CnkRbl_PatClose,  
            NULL,
            -1
        },
        {
            CNK_LANG_RBL_PAT,
            0,
            FMT_DEF_OUTDENT,
            "pat",
            NULL,
            CnkRbl_Pat,  
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

    Lookup *lk = Lookup_Make(ctx->m, CNK_LANG_RBL_START, NULL, NULL);
    FmtCnf *cnf = configs;
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

        printf("Adding %s\n", CnkLang_RangeToChars(cnf->id));
        r |= Lookup_Add(ctx->m, lk, cnf->id, def);
        cnf++;
    }

    return Fmt_Add(ctx->m, ctx, lk);
}
