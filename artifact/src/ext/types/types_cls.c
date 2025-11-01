#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;
ClassDef *ObjectCls = NULL;

static i64 Hashed_Print(Buff *bf, Abstract *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(bf->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(bf->m, h->value, 0);
        word typeOf = TYPE_UNKNOWN;
        if(h->value != NULL){
            typeOf = h->value->type.of;
            if(typeOf == TYPE_OBJECT){
                typeOf = ((Object *)h->value)->objType.of;
            }
        }
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, h->orderIdx), 
            (Abstract *)I32_Wrapped(bf->m, h->idx), 
            (Abstract *)wid, 
            h->key, 
            (Abstract *)val, 
            (Abstract *)Type_ToStr(bf->m, typeOf),
            NULL
        };
        return Fmt(bf, "H<$,$ $/@ -> $/$>", args);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, h->idx), 
            h->key, 
            (Abstract *)h->value, 
            NULL
        };
        return Fmt(bf, "H<$ @ -> @>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}


static Abstract *Object_ByKey(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    Object *obj = (Object *)as(data, TYPE_OBJECT);
    return Object_Get(obj, (Abstract *)fg->key);
}

static Abstract *Object_ByIdx(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    Object *obj = (Object *)as(data, TYPE_OBJECT);
    return Span_Get(obj->order, fg->idx);
    return NULL;
}

static i32 _objIndent = 0;

static i64 Object_Print(Buff *bf, Abstract *a, cls type, word flags){
    Object *obj = (Object *)as(a, TYPE_OBJECT);
    ClassDef *cls = Lookup_Get(ClassLookup, obj->objType.of);
    Abstract *args[4];
    i32 dataCount = obj->order->nvalues;
    if(flags & (MORE|DEBUG)){
        i64 total = 0;
        if(cls == NULL){
            total += Fmt(bf, "Object<", args);
        }else{
            dataCount = dataCount - obj->propMask;

            args[0] = (Abstract *)cls->name;
            args[1] = NULL;
            total += Fmt(bf, "$<", args);

            Iter it;
            Iter_Init(&it, cls->propOrder);
            while((Iter_Next(&it) & END) == 0){
                Hashed *h = Span_Get(obj->order, it.idx);
                ToS(bf, h->key, ZERO, MORE); 
                Buff_AddBytes(bf, (byte *)": ", 2);
                ToS(bf, h->value, ZERO, flags); 
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
        }

        if(dataCount){
            args[0] = (Abstract *)I32_Wrapped(bf->m, dataCount);
            args[1] = NULL;
            total += Fmt(bf, " ^D.$^d.nvalues {", args);
            Iter *it = (Iter *)as(
                Object_GetIter(Object_GetMem(obj), NULL, (Abstract *)obj, NULL),
                TYPE_ITER);
            while((Iter_Next(it) & END) == 0){
                if(flags & DEBUG){
                    total += Buff_AddBytes(bf, (byte *)"\n  ", 3);
                }
                Hashed *h = (Hashed *)Iter_Get(it);
                if(h->value != NULL && h->value->type.of == TYPE_OBJECT){
                    _objIndent++;
                    i32 i = _objIndent;
                    total += Buff_AddBytes(bf, (byte *)"\n", 1);
                    while(i--){
                        total += Buff_AddBytes(bf, (byte *)"  ", 2);
                    }
                }
                total += ToS(bf, h->key, 0, MORE); 
                total += Buff_AddBytes(bf, (byte *)" -> ", 4);
                if(flags & (MORE|DEBUG)){
                    total += ToS(bf, h->value, 0, flags);  
                }else{
                    Str *typeStr = NULL;
                    if(h->value != NULL){
                        typeStr = Type_ToStr(bf->m, h->value->type.of);
                    }
                    total += ToS(bf, (Abstract *)typeStr, 0, MORE);
                }
                if(h->value != NULL && h->value->type.of == TYPE_OBJECT){
                    _objIndent--;
                }
                if((it->type.state & LAST) == 0){
                    total += Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
            if(flags & DEBUG){
                total += Buff_AddBytes(bf, (byte *)"\n", 1);
            }
            total += Buff_AddBytes(bf, (byte *)"}", 1);
        }
        total += Buff_AddBytes(bf, (byte *)">", 1);
        return total;
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static boolean FetchTarget_Exact(FetchTarget  *a, FetchTarget *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS) ||
            a->idx != b->idx || a->offset != b->offset || 
            !Exact((Abstract *)a->key, (Abstract *)b->key)){
        return FALSE;
    }
    return TRUE;
}

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS)){
        return FALSE;
    }
    return Exact((Abstract *)a->val.targets, (Abstract *)b->val.targets);
}

static status Fetcher_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    Fetcher *fch = (Fetcher *)as(a, TYPE_FETCHER);
    Abstract *args[] = {
        (Abstract *)Type_StateVec(bf->m, fch->type.of, fch->type.state),
        NULL,
    };
    total += Fmt(bf, "Fetcher<@ ", args);
    Abstract *args1[] = {
        (Abstract *)fch->val.targets,
        NULL,
    };
    total += Fmt(bf, "@>", args1);

    return total;
}

static status FetchTargetFunc_Print(Buff *bf, Abstract *a, cls type, word flags){
    return Fmt(bf, "FTFunc<>", NULL);
}

static status FetchTarget_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    FetchTarget *tg = (FetchTarget *)as(a, TYPE_FETCH_TARGET);
    Abstract *args[] = {
        (Abstract *)Type_StateVec(bf->m, tg->type.of, tg->type.state),
        NULL,
    };
    total += Fmt(bf, "FT<@", args);
    if(tg->objType.of != ZERO){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(bf->m, tg->objType.of),
            NULL
        };
        total += Fmt(bf, " $ ", args);

    }
    if(tg->key != NULL){
        Abstract *args[] = {
            (Abstract *)tg->key,
            NULL
        };
        total += Fmt(bf, " @", args);
    }
    if(tg->type.state & FETCH_TARGET_IDX || tg->idx != -1){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, tg->idx),
            NULL
        };
        total += Fmt(bf, " idx^D.$^d.", args);
    }
    if(tg->type.state & FETCH_TARGET_RESOLVED){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(bf->m, tg->objType.of),
            (Abstract *)I16_Wrapped(bf->m, tg->offset),
            NULL
        };
        total += Fmt(bf, " -> $/offset^D.$^d.>", args);
    }
    if(tg->type.state & FETCH_TARGET_FUNC){
        Abstract *args[] = {
            (Abstract *)Ptr_Wrapped(bf->m, tg->func, TYPE_FETCH_FUNC),
            NULL
        };
        total += Fmt(bf, "func/@", args);
    }
    total += Buff_AddBytes(bf, (byte *)">", 1);

    return total;
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
    fetchTargetLabels[16] = Str_CstrRef(m, "COMMAND");
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
    cls->objType.of = TYPE_OBJECT;
    cls->name = Str_FromCstr(m, "Object", STRING_COPY);
    ObjectCls = cls;

    Object obj;
    Table_Set(cls->atts, (Abstract *)Str_FromCstr(m, "tbl", STRING_COPY),
        (Abstract *)I16_Wrapped(m, (void *)(&obj.tbl)-(void *)(&obj)));
    Table_Set(cls->atts, (Abstract *)Str_FromCstr(m, "order", STRING_COPY),
        (Abstract *)I16_Wrapped(m, (void *)(&obj.order)-(void *)(&obj)));
    r |= Class_Register(m, cls);

    /* overide hashed print */
    r |= Lookup_Add(m, ToStreamLookup, TYPE_HASHED, (void *)Hashed_Print); 

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

