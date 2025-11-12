#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;
ClassDef *ObjectCls = NULL;

static i64 Hashed_Print(Buff *bf, void *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(bf->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(bf->m, h->value, 0);
        word typeOf = TYPE_UNKNOWN;
        if(h->value != NULL){
            typeOf = ((Abstract *)h->value)->type.of;
        }
        void *args[] = {
            I32_Wrapped(bf->m, h->orderIdx), 
            I32_Wrapped(bf->m, h->idx), 
            wid, 
            h->key, 
            val, 
            Type_ToStr(bf->m, typeOf),
            NULL
        };
        return Fmt(bf, "H<$,$ $/@ -> $/$>", args);
    }else if(flags & MORE){
        void *args[] = {
            I32_Wrapped(bf->m, h->idx), 
            h->key, 
            h->value, 
            NULL
        };
        return Fmt(bf, "H<$ @ -> @>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}


static void *Object_ByKey(MemCh *m, FetchTarget *fg, void *data, void *source){
    Object *obj = (Object *)as(data, TYPE_OBJECT);
    return Object_Get(obj, fg->key);
}

static void *Object_ByIdx(MemCh *m, FetchTarget *fg, void *data, void *source){
    Object *obj = (Object *)as(data, TYPE_OBJECT);
    return Span_Get(obj->order, fg->idx);
    return NULL;
}

static status Object_ToBinSeg(BinSegCtx *ctx, void *a, i16 id){
    return NOOP;
}

static boolean FetchTarget_Exact(FetchTarget  *a, FetchTarget *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS) ||
            a->idx != b->idx || a->offset != b->offset || 
            !Exact(a->key, b->key)){
        return FALSE;
    }
    return TRUE;
}

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS)){
        return FALSE;
    }
    return Exact(a->val.targets, b->val.targets);
}

static status Fetcher_Print(Buff *bf, void *a, cls type, word flags){
    Fetcher *fch = (Fetcher *)as(a, TYPE_FETCHER);
    void *args[] = {
        Type_StateVec(bf->m, fch->type.of, fch->type.state),
        NULL,
    };
    Fmt(bf, "Fetcher<@ ", args);
    void *args1[] = {
        fch->val.targets,
        NULL,
    };
    return Fmt(bf, "@>", args1);
}

static status FetchTargetFunc_Print(Buff *bf, void *a, cls type, word flags){
    return Fmt(bf, "FTFunc<>", NULL);
}

static status FetchTarget_Print(Buff *bf, void *a, cls type, word flags){
    FetchTarget *tg = (FetchTarget *)as(a, TYPE_FETCH_TARGET);
    void *args[] = {
        Type_StateVec(bf->m, tg->type.of, tg->type.state),
        NULL,
    };
    Fmt(bf, "FT<@", args);
    if(tg->type.of != ZERO){
        void *args[] = {
            Type_ToStr(bf->m, tg->type.of),
            NULL
        };
        Fmt(bf, " $ ", args);

    }
    if(tg->key != NULL){
        void *args[] = {
            tg->key,
            NULL
        };
        Fmt(bf, " key=@", args);
    }
    if(tg->type.state & FETCH_TARGET_IDX || tg->idx != -1){
        void *args[] = {
            I32_Wrapped(bf->m, tg->idx),
            NULL
        };
        Fmt(bf, " idx^D.$^d.", args);
    }
    if(tg->type.state & FETCH_TARGET_RESOLVED){
        void *args[] = {
            Type_ToStr(bf->m, tg->type.of),
            I16_Wrapped(bf->m, tg->offset),
            NULL
        };
        Fmt(bf, " -> $/offset^D.$^d.>", args);
    }
    if(tg->type.state & FETCH_TARGET_FUNC){
        void *args[] = {
            Ptr_Wrapped(bf->m, tg->func, TYPE_FETCH_FUNC),
            NULL
        };
        Fmt(bf, "func/@", args);
    }
    return Buff_AddBytes(bf, (byte *)">", 1);
}

status Object_Print(Buff *bf, void *a, cls type, word flags){
    printf("\nOP\n");
    fflush(stdout);
    static i32 _objIndent = 0;
    Object *obj = NULL;
    if(((Abstract *)a)->type.of & TYPE_OBJECT){
        obj = (Object *)a;
    }else{
        as(a, TYPE_OBJECT);
    }
    ClassDef *cls = Lookup_Get(ClassLookup, obj->type.of);
    void *args[4];
    i32 dataCount = obj->order->nvalues;
    MemCh *m = Object_GetMem(obj);
    if(flags & MORE){
        if(cls == NULL){
            args[0] = Type_StateVec(m, obj->type.of, obj->type.state);
            Fmt(bf, "Object<@", args);
        }else{
            dataCount = dataCount - obj->propMask;

            args[0] = cls->name;
            args[1] = Type_StateVec(m, obj->type.of, obj->type.state);
            args[2] = NULL;
            Fmt(bf, "$<@", args);
            Iter it;
            Iter_Init(&it, cls->propOrder);
            while((Iter_Next(&it) & END) == 0){
                Hashed *h = Span_Get(obj->order, it.idx);
                ToS(bf, h->key, ZERO, flags); 
                Buff_AddBytes(bf, (byte *)": ", 2);
                ToS(bf, h->value, ZERO, flags); 
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
        }

        if(dataCount){
            args[0] = I32_Wrapped(bf->m, dataCount);
            args[1] = NULL;
            Fmt(bf, " ^D.$^d.nvalues {", args);
            Iter *it = (Iter *)as(
                Object_GetIter(Object_GetMem(obj), NULL, obj, NULL),
                TYPE_ITER);
            while((Iter_Next(it) & END) == 0){
                if(flags & DEBUG){
                    Buff_AddBytes(bf, (byte *)"\n  ", 3);
                }
                Hashed *h = (Hashed *)Iter_Get(it);
                if(h->value != NULL && ((Abstract *)h->value)->type.of == TYPE_OBJECT){
                    _objIndent++;
                    i32 i = _objIndent;
                    Buff_AddBytes(bf, (byte *)"\n", 1);
                    while(i--){
                        Buff_AddBytes(bf, (byte *)"  ", 2);
                    }
                }
                ToS(bf, h->key, 0, flags); 
                Buff_AddBytes(bf, (byte *)" -> ", 4);
                if(flags & (MORE|DEBUG)){
                    printf("\nhere %p\n", h);
                    fflush(stdout);
                    printf("\nhere II %p\n", h->value);
                    fflush(stdout);
                    ToS(bf, h->value, 0, flags);  
                }else{
                    Str *typeStr = NULL;
                    if(h->value != NULL){
                        typeStr = Type_ToStr(bf->m, ((Abstract *)h->value)->type.of);
                    }
                    ToS(bf, typeStr, 0, flags);
                }
                if(h->value != NULL && ((Abstract *)h->value)->type.of == TYPE_OBJECT){
                    _objIndent--;
                }
                if((it->type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
            if(flags & DEBUG){
                Buff_AddBytes(bf, (byte *)"\n", 1);
            }
            Buff_AddBytes(bf, (byte *)"}", 1);
        }
        return Buff_AddBytes(bf, (byte *)">", 1);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

status Types_ClsInit(MemCh *m){
    status r = READY;
    Lookup *lk = NULL;
    if(_init){
        r |= NOOP;
        return r;
    }
    _init = TRUE;

    /* labels */
    lk = ToSFlagLookup;

    Str **fetcherLabels = (Str **)Arr_Make(m, 17);
    fetcherLabels[9] = Str_CstrRef(m, "VAR");
    fetcherLabels[10] = Str_CstrRef(m, "FOR");
    fetcherLabels[11] = Str_CstrRef(m, "WITH");
    fetcherLabels[12] = Str_CstrRef(m, "IF");
    fetcherLabels[13] = Str_CstrRef(m, "IFNOT");
    fetcherLabels[14] = Str_CstrRef(m, "END");
    fetcherLabels[15] = Str_CstrRef(m, "COMMAND");
    fetcherLabels[16] = Str_CstrRef(m, "TEMPL");
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)fetcherLabels);

    Str **fetchTargetLabels = (Str **)Arr_Make(m, 17);
    fetchTargetLabels[9] = Str_CstrRef(m, "ATT");
    fetchTargetLabels[10] = Str_CstrRef(m, "KEY");
    fetchTargetLabels[11] = Str_CstrRef(m, "IDX");
    fetchTargetLabels[12] = Str_CstrRef(m, "ITER");
    fetchTargetLabels[13] = Str_CstrRef(m, "PROP");
    fetchTargetLabels[14] = Str_CstrRef(m, "FUNC");
    fetchTargetLabels[15] = Str_CstrRef(m, "RESOLVED");
    fetchTargetLabels[16] = Str_CstrRef(m, "HASH");
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)fetchTargetLabels);

    /* ToS */
    lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Print);
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)FetchTarget_Print);
    r |= Lookup_Add(m, lk, TYPE_FETCH_FUNC, (void *)FetchTargetFunc_Print);

    /* exact */
    lk = ExactLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Exact); 
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)FetchTarget_Exact); 

    /* Object class def */
    ClassDef *cls = ClassDef_Make(m);
    cls->api.byKey = Object_ByKey;
    cls->api.byIdx = Object_ByIdx;
    cls->api.getIter = Object_GetIter;
    cls->api.toS = Object_Print;
    cls->type.of = TYPE_OBJECT;
    cls->name = Str_FromCstr(m, "Object", STRING_COPY);
    ObjectCls = cls;

    Object obj;
    Table_Set(cls->atts, Str_FromCstr(m, "tbl", STRING_COPY),
        I16_Wrapped(m, (void *)(&obj.tbl)-(void *)(&obj)));
    Table_Set(cls->atts, Str_FromCstr(m, "order", STRING_COPY),
        I16_Wrapped(m, (void *)(&obj.order)-(void *)(&obj)));
    r |= Class_Register(m, cls);

    /* overide hashed print */
    r |= Lookup_Add(m, ToStreamLookup, TYPE_HASHED, (void *)Hashed_Print); 

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

