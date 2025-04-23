#include <external.h>
#include <caneka.h>

static i64 Wrapped_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)asIfc(a, TYPE_WRAPPED);
    void *args = {Type_ToChars(sg->type.of), NULL};
    return Out("Wr<_c>", args);
}

static i64 Wrapped_Do(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_DO);
    void *args[] = {Type_ToChars(sg->type.of), sg->val.dof, NULL};
    return Out("Wdo<_c _a>", args);
}

static i64 Wrapped_Ptr(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    void *args = {Type_ToChars(sg->type.of), sg->val.ptr, NULL};
    return Out("Wptr<_c _a>", args);
}

static i64 WrappedUtil_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    void *args = {Type_ToChars(sg->type.of), (void *)&sg->val.value, NULL};
    return Out("Wu<_c ^D._i8^d.>", args);
}

static i64 WrappedI64_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    void *args = {Type_ToChars(sg->type.of), (void *)&sg->val.value, NULL};
    return Out("Wi64<_c ^D._i8^d.>", args);
}

static i64 WrappedTime64_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    void *args = {Type_ToChars(sg->type.of), (void *)&sg->val.value, NULL};
    return Out("Wt64<_c ^D._i8^d.>", args);
}

static i64 Abstract_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    void *args = {Type_ToChars(a->type.of),NULL};
    return Out("A<_c>", args);
}

static i64 Single_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Single *sg = (Single *)as(a, TYPE_SINGLE);
    void *args = {Type_ToChars(sg->type.of),NULL};
    return Out("Single<_c>", args);
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
