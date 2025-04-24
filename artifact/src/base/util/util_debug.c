#include <external.h>
#include <caneka.h>

static i64 Wrapped_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)asIfc(a, TYPE_WRAPPED);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        NULL
    };
    return Out("Wr<$>", args);
}

static i64 Wrapped_Do(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_DO);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        (Abstract *)a,
        NULL
    };
    return Out("Wdo<$ $>", args);
}

static i64 Wrapped_Ptr(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        (Abstract *)a,
        NULL
    };
    return Out("Wptr<$ $>", args);
}

static i64 WrappedUtil_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        (Abstract *)a,
        NULL
    };
    return Out("Wu<$ ^D.$^d.>", args);
}

static i64 WrappedI64_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        (Abstract *)a,
        NULL
    };
    return Out("Wi64<$ ^D.$^d.>", args);
}

static i64 WrappedTime64_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        (Abstract *)a,
        NULL
    };
    return Out("Wt64<$ ^D.$^d.>", args);
}

static i64 Abstract_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        NULL
    };
    return Out("A<$>", args);
}

static i64 Single_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_SINGLE);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
        NULL
    };
    return Out("Single<$>", args);
}

status UtilDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_SINGLE, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)WrappedI64_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_TIME64, (void *)WrappedTime64_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_DO, (void *)Wrapped_Do);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_PTR, (void *)Wrapped_Ptr);
    return r;
}
