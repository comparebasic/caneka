#include <external.h>
#include <caneka.h>

static i64 Wrapped_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)asIfc(a, TYPE_WRAPPED);
    if(flags & (MORE|DEBUG)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Out("Wr<$>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 Wrapped_Do(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_DO);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            (Abstract *)a,
            NULL
        };
        return Out("Wdo<$ $>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 Wrapped_Ptr(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            (Abstract *)a,
            NULL
        };
        return Out("Wptr<$ $>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 WrappedUtil_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    Str *s = Str_FromI64(sm->m, sg->val.value);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Out("Wu<^D.@^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI64_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    Str *s = Str_FromI64(sm->m, sg->val.value);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Out("Wi64<^D.@^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedTime64_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    Str *s = Time64_ToStr(sm->m, sg->val.value);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            (Abstract *)s,
            NULL
        };
        return Out("Wt64<$ ^D.@^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 Abstract_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Out("A<$>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 Single_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_SINGLE);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Out("Single<$>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

status Util_ToSInit(MemCh *m, Lookup *lk){
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
