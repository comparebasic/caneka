#include <external.h>
#include <caneka.h>

Abstract *CnkRbl_Pat(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    String *out = NULL;
    if(def->id == CNK_LANG_RBL_PAT){
        Debug_Print((void *)key, 0, "Found Pat: ", COLOR_PURPLE, TRUE);
        printf("\n");

        FmtItem *item = fmt->item;
        if(item == NULL){
            Fatal("Need item to add pattern to", TYPE_LANG_CNK_RBL);
            return NULL;
        }

        IterStr it;
        byte out;
        IterStr_Init(&it, key, sizeof(byte), bytes(&out));
        
        PatCharDef pat;
        while((IterStr_Next(&it) & END) == 0){
            byte *b = (byte *)IterStr_Get(&it);
            pat.flags = PAT_TERM;
            pat.from = *b;
            pat.to = *b;
            String_AddBytes(m, item->content, bytes(&pat), sizeof(PatCharDef));
        }
    }else if(def->id == CNK_LANG_RBL_START){
        Debug_Print((void *)key, 0, "Rendering Pat: ", COLOR_PURPLE, TRUE);
        printf("\n");

        FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
        out = String_Init(m, STRING_EXTEND);
        char *cstr = "static ";
        String_AddBytes(m, out, bytes(cstr), strlen(cstr));
        String_Add(m, out, key);
        cstr = " {\n";
        String_AddBytes(m, out, bytes(cstr), strlen(cstr));

        IterStr its;
        PatCharDef pout;
        IterStr_Init(&its, item->content, sizeof(PatCharDef), bytes(&pout));
        while((IterStr_Next(&its) & END) == 0){
            PatCharDef *pat = (PatCharDef *)IterStr_Get(&its);
            cstr = "    ";
            String_AddBytes(m, out, bytes(cstr), strlen(cstr));
            String_AddInt(m, out, pat->flags);
            cstr = ", '";
            String_AddBytes(m, out, bytes(cstr), strlen(cstr));
            String_AddBytes(m, out, (byte *)&pat->from, 1);
            cstr = "', ";
            String_AddBytes(m, out, bytes(cstr), strlen(cstr));
            String_AddBytes(m, out, (byte *)&pat->to, 1);
            cstr = "'";
            String_AddBytes(m, out, bytes(cstr), strlen(cstr));
            if((pat->flags & PAT_TERM) != 0){
                cstr = "\n";
                String_AddBytes(m, out, bytes(cstr), strlen(cstr));
            }
        }
        cstr = "}\n";
        String_AddBytes(m, out, bytes(cstr), strlen(cstr));
    }

    return (Abstract *)out;
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
    fmt->item->key = s;

    String *key = String_Clone(m, s);

    String_AddBytes(m, key, bytes("_pat"), 4);
    Span *sp = (Span *)asIfc(fmt->root->value, TYPE_SPAN);
    String_AddInt(m, key, sp->nvalues);

    printf("Setting Table Entry for item of space %s\n", CnkLang_RangeToChars(fmt->item->def->id));

    Table_Set(sp, (Abstract *)key,  (Abstract *)fmt->item);

    return NULL;
}

Abstract *CnkRbl_Out(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a){
    Span *sp = (Span *)asIfc(fmt->root->value, TYPE_SPAN);
    Debug_Print((void *)sp, 0, "Patterns: ", COLOR_PURPLE, TRUE);
    printf("\n");

    String *out = String_Init(m, STRING_EXTEND);
    Iter it;
    Iter_Init(&it, sp);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            FmtItem *item = (FmtItem *)h->value;
            if(item->def->to == CnkRbl_Out){
                printf("oops wrong item\n");
                continue;
            }
            String_Add(m, out, 
                (String *)item->def->to(m, def, fmt, (String *)h->item, (Abstract *)item));
        }
    }

    return (Abstract *)out;
}

status CnkRblLang_AddDefs(FmtCtx *ctx){
    status r = READY;
    MemCtx *m = ctx->m;

    FmtCnf configs[] = {
        {
            CNK_LANG_RBL_START,
            0,
            FMT_DEF_TO_ON_CLOSE,
            "",
            NULL,
            CnkRbl_Out,  
            NULL,
            -1
        },
        {
            CNK_LANG_RBL_OPEN_CLOSE,
            0,
            FMT_DEF_TO_ON_CLOSE,
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
