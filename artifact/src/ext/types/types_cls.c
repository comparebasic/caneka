#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;
ClassDef *ObjectCls = NULL;

static i64 Hashed_Print(Stream *sm, Abstract *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(sm->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(sm->m, h->value, 0);
        word typeOf = TYPE_UNKNOWN;
        if(h->value != NULL){
            typeOf = h->value->type.of;
            if(typeOf == TYPE_OBJECT){
                typeOf = ((Object *)h->value)->objType.of;
            }
        }
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, h->orderIdx), 
            (Abstract *)I32_Wrapped(sm->m, h->idx), 
            (Abstract *)wid, 
            h->key, 
            (Abstract *)val, 
            (Abstract *)Type_ToStr(sm->m, typeOf),
            NULL
        };
        return Fmt(sm, "H<$,$ $/@ -> $/$>", args);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, h->idx), 
            h->key, 
            (Abstract *)h->value, 
            NULL
        };
        return Fmt(sm, "H<$ @ -> @>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
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

static i64 Object_Print(Stream *sm, Abstract *a, cls type, word flags){
    Object *obj = (Object *)as(a, TYPE_OBJECT);
    ClassDef *cls = Lookup_Get(ClassLookup, obj->objType.of);
    Abstract *args[4];
    i32 dataCount = obj->order->nvalues;
    if(flags & (MORE|DEBUG)){
        i64 total = 0;
        if(cls == NULL){
            total += Fmt(sm, "Object<", args);
        }else{
            dataCount = dataCount - obj->propMask;

            args[0] = (Abstract *)cls->name;
            args[1] = NULL;
            total += Fmt(sm, "$<", args);

            Iter it;
            Iter_Init(&it, cls->propOrder);
            while((Iter_Next(&it) & END) == 0){
                Hashed *h = Span_Get(obj->order, it.idx);
                ToS(sm, h->key, ZERO, MORE); 
                Stream_Bytes(sm, (byte *)": ", 2);
                ToS(sm, h->value, ZERO, flags); 
                if((it.type.state & LAST) == 0){
                    Stream_Bytes(sm, (byte *)", ", 2);
                }
            }
        }

        if(dataCount){
            args[0] = (Abstract *)I32_Wrapped(sm->m, dataCount);
            args[1] = NULL;
            total += Fmt(sm, "^D.$^d.nvalues {", args);
            Iter *it = (Iter *)as(
                Object_GetIter(Object_GetMem(obj), NULL, (Abstract *)obj, NULL),
                TYPE_ITER);
            while((Iter_Next(it) & END) == 0){
                if(flags & DEBUG){
                    total += Stream_Bytes(sm, (byte *)"\n  ", 3);
                }
                Hashed *h = (Hashed *)Iter_Get(it);
                total += ToS(sm, h->key, 0, MORE); 
                total += Stream_Bytes(sm, (byte *)" -> ", 4);
                total += ToS(sm, h->value, 0, flags);  
                if((it->type.state & LAST) == 0){
                    total += Stream_Bytes(sm, (byte *)", ", 2);
                }
            }
            if(flags & DEBUG){
                total += Stream_Bytes(sm, (byte *)"\n", 1);
            }
            total += Stream_Bytes(sm, (byte *)"}", 1);
        }
        total += Stream_Bytes(sm, (byte *)">", 1);
        return total;
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
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

static status Fetcher_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    Fetcher *fch = (Fetcher *)as(a, TYPE_FETCHER);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)fch, ToS_FlagLabels),
        NULL,
    };
    total += Fmt(sm, "Fetcher<$ ", args);
    Abstract *args1[] = {
        (Abstract *)fch->val.targets,
        NULL,
    };
    total += Fmt(sm, "@>", args1);

    return total;
}

static status FetchTargetFunc_Print(Stream *sm, Abstract *a, cls type, word flags){
    return Fmt(sm, "FTFunc<>", NULL);
}

static status FetchTarget_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    FetchTarget *tg = (FetchTarget *)as(a, TYPE_FETCH_TARGET);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tg, ToS_FlagLabels),
        NULL,
    };
    total += Fmt(sm, "FT<$", args);
    if(tg->objType.of != ZERO){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(sm->m, tg->objType.of),
            NULL
        };
        total += Fmt(sm, " $ ", args);

    }
    if(tg->key != NULL){
        Abstract *args[] = {
            (Abstract *)tg->key,
            NULL
        };
        total += Fmt(sm, " @", args);
    }
    if(tg->type.state & FETCH_TARGET_IDX || tg->idx != -1){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, tg->idx),
            NULL
        };
        total += Fmt(sm, " idx^D.$^d.", args);
    }
    if(tg->type.state & FETCH_TARGET_RESOLVED){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(sm->m, tg->objType.of),
            (Abstract *)I16_Wrapped(sm->m, tg->offset),
            NULL
        };
        total += Fmt(sm, " -> $/offset^D.$^d.>", args);
    }
    if(tg->type.state & FETCH_TARGET_FUNC){
        Abstract *args[] = {
            (Abstract *)Ptr_Wrapped(sm->m, tg->func, TYPE_FETCH_FUNC),
            NULL
        };
        total += Fmt(sm, "func/@", args);
    }
    total += Stream_Bytes(sm, (byte *)">", 1);

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
    ObjectCls = cls;

    Object obj;
    Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "tbl"),
        (Abstract *)I16_Wrapped(m, (void *)(&obj.tbl)-(void *)(&obj)));
    Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "order"),
        (Abstract *)I16_Wrapped(m, (void *)(&obj.order)-(void *)(&obj)));
    r |= Class_Register(m, cls);

    /* overide hashed print */
    r |= Lookup_Add(m, ToStreamLookup, TYPE_HASHED, (void *)Hashed_Print); 

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

