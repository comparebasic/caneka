#include <external.h>
#include <caneka.h>

Abstract *CnkRbl_Pat(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    DebugStack_Push(key, key->type.of); 
    Span *p = (Span *)asIfc(fmt->item->value, TYPE_SPAN);

    FmtItem *item = Span_Get(p, p->metrics.set);
    if(item == NULL){
        FmtDef_PushItem(fmt, def->id, NULL, def);
        item = fmt->item;
        item->key = String_Init(m, STRING_EXTEND);
        String_Add(m, item->key, item->parent->key);
        char *cstr = "_pat";
        String_AddBytes(m, item->key, bytes(cstr), strlen(cstr));
        String_AddInt(m, item->key, p->nvalues-1);

        Span_Add(p, (Abstract *)item);
    }

    if(item == NULL){
        Fatal("Need item to add pattern to", TYPE_LANG_CNK_RBL);
        DebugStack_Pop();
        return NULL;
    }

    IterStr it;
    IterStr_Init(&it, key, sizeof(byte));
    item->content->type.state |= FLAG_STRING_CONTIGUOUS;
    
    PatCharDef pat;
    while((IterStr_Next(&it) & END) == 0){
        byte *b = (byte *)IterStr_Get(&it);
        pat.flags = PAT_TERM;
        pat.from = *b;
        pat.to = *b;
        String_AddBytes(m, item->content, bytes(&pat), sizeof(PatCharDef));
    }

    DebugStack_Pop();
    return NULL;
}

Abstract *CnkRbl_PatClose(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a){
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

    fmt->item = fmt->item->parent;

    return NULL;
}

Abstract *CnkRbl_PatKeyOpen(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    DebugStack_Push(key, key->type.of); 
    if(def->id == CNK_LANG_RBL_PAT_KEY){
        fmt->item->key = key;
        Span *sp = (Span *)asIfc(fmt->root->value, TYPE_TABLE);
        fmt->item->value = (Abstract *)Span_Make(m, TYPE_SPAN);

        Table_Set(sp, (Abstract *)key,  (Abstract *)fmt->item);

        DebugStack_Pop();
        return NULL;
    }else{
        Span *p = (Span *)asIfc(fmt->item->value, TYPE_SPAN);
        String *out = String_Init(m, STRING_EXTEND);

        Iter it;
        Iter_Init(&it, p);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = (Hashed *)Iter_Get(&it);
            if(h != NULL){
                FmtItem *item = (FmtItem *)h->value;

                Iter patIt;
                Iter_Init(&patIt, (Span *)asIfc(item->value, TYPE_SPAN));
                while((Iter_Next(&patIt) & END) == 0){
                    FmtItem *item = (FmtItem *)as(Iter_Get(&patIt), TYPE_FMT_ITEM);

                    char *cstr = "static PatCharDef ";
                    String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                    String_Add(m, out, item->key);
                    cstr = "[] = {\n    ";
                    String_AddBytes(m, out, bytes(cstr), strlen(cstr));

                    IterStr its;
                    IterStr_Init(&its, item->content, sizeof(PatCharDef));
                    while((IterStr_Next(&its) & END) == 0){
                        PatCharDef *pat = (PatCharDef *)IterStr_Get(&its);
                        cstr = "{";
                        String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                        Match_AddFlagsToStr(m, out, pat->flags);
                        cstr = ", ";
                        String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                        String_AddAsciiSrc(m, out, (byte)pat->from);
                        cstr = ", ";
                        String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                        String_AddAsciiSrc(m, out, (byte)pat->to);
                        cstr = "}";
                        String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                        if((pat->flags & PAT_TERM) != 0){
                            cstr = ",\n";
                            String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                            if((its.type.state & FLAG_ITER_LAST) == 0){
                                cstr = "    ";
                                String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                            }
                        }else{
                            if((its.type.state & FLAG_ITER_LAST) == 0){
                                cstr = ", ";
                                String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                            }else{
                                cstr = "\n";
                                String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                            }
                        }
                    }
                    cstr = "};\n";
                    String_AddBytes(m, out, bytes(cstr), strlen(cstr));
                }
            }
        }

        DebugStack_Pop();
        return (Abstract *)out;
    }
}

Abstract *CnkRbl_Out(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a){
    DebugStack_Push(s, s->type.of); 
    Span *sp = (Span *)asIfc(fmt->root->value, TYPE_SPAN);

    String *out = String_Init(m, STRING_EXTEND);
    Iter it;
    Iter_Init(&it, sp);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            FmtItem *item = (FmtItem *)h->value;
            if(item->def->to == CnkRbl_Out){
                continue;
            }
            String_Add(m, out, 
                (String *)item->def->to(m, def, fmt, (String *)h->item, (Abstract *)item));
        }
    }

    DebugStack_Pop();
    return (Abstract *)out;
}

status CnkRblLang_AddDefs(FmtCtx *ctx){
    status r = READY;
    MemCtx *m = ctx->m;

    Lookup *lk = Lookup_Make(ctx->m, CNK_LANG_RBL_START, NULL, NULL);
    FmtCnf *cnf = cnk_roebling_configs;
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

        r |= Lookup_Add(ctx->m, lk, cnf->id, def);
        cnf++;
    }

    return Fmt_Add(ctx->m, ctx, lk);
}
