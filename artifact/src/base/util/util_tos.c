#include <external.h>
#include <caneka.h>

static i64 Wrapped_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)asIfc(a, TYPE_WRAPPED);
    if(flags & (MORE|DEBUG)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Fmt(sm, "Wr\\<$>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 WrappedDo_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_DO);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            (Abstract *)I64_Wrapped(sm->m, (util)sg->val.ptr),
            NULL
        };
        return Fmt(sm, "Wdo\\<$ ^D.*$^d.>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

static i64 Wrapped_Ptr(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    word fl = (DEBUG|MORE);
    if((flags & fl) == fl){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(sg->type.of)),
            (Abstract *)a,
            NULL
        };
        return Fmt(sm, "Wptr\\<$ $>", args);
    }else{
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(sm->m, (util)a),
            NULL
        };
        return Fmt(sm, "*$", args);
    }
}

static i64 WrappedUtil_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    Str *s = Str_FromI64(sm->m, sg->val.value);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(sm, "Wu\\<^D.@^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedMemCount_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_MEMCOUNT);
    Str *s = Str_MemCount(sm->m, sg->val.value);
    return Stream_Bytes(sm, s->bytes, s->length);
}

static i64 WrappedI64_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    Str *s = Str_FromI64(sm->m, sg->val.value);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(sm, "Wi64\\<^D.$^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI32_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I32);
    Str *s = Str_FromI64(sm->m, (i64)sg->val.i);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(sm, "Wi32\\<^D.$^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI16_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I16);
    Str *s = Str_FromI64(sm->m, (i64)sg->val.w);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(sm, "Wi16\\<^D.$^d.>", args);
    }else{
        return ToS(sm, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI8_Print(Stream *sm, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I8);
    Str *s = Str_FromI64(sm->m, (i64)sg->val.b);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(sm, "Wi8\\<^D.^d.>", args);
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
        return Fmt(sm, "Wt64\\<$ ^D.@^d.>", args);
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
        return Fmt(sm, "A\\<$>", args);
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
        return Fmt(sm, "Single\\<$>", args);
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
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I32, (void *)WrappedI32_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I16, (void *)WrappedI16_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I8, (void *)WrappedI8_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_TIME64, (void *)WrappedTime64_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_DO, (void *)WrappedDo_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_PTR, (void *)Wrapped_Ptr);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_MEMCOUNT, (void *)WrappedMemCount_Print);
    return r;
}
