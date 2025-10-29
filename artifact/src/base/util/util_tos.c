#include <external.h>
#include <caneka.h>

static i64 Wrapped_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)asIfc(a, TYPE_WRAPPED);
    if(flags & (MORE|DEBUG)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Fmt(bf, "Wr\\<$>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static i64 WrappedDo_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_DO);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            (Abstract *)I64_Wrapped(bf->m, (util)sg->val.ptr),
            NULL
        };
        return Fmt(bf, "Wdo\\<$ ^D.*$^d.>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static i64 WrappedFunc_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_FUNC);
    Abstract *args[] = {
        (Abstract *)Type_StateVec(bf->m, sg->type.of, sg->type.state),
        (Abstract *)Util_Wrapped(bf->m, (util)sg->val.ptr),
        NULL
    };
    if(flags & MORE){
        return Fmt(bf, "Wfunc\\<$ ^D.$^d.>", args);
    }else{
        return Fmt(bf, "Wfunc\\<$ $>", args);
    }
}

static i64 WrappedPtr_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    word fl = (DEBUG|MORE);
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(bf->m, Type_ToChars(sg->objType.of)),
        NULL
    };
    total += Fmt(bf, "Wptr\\<$ ", args);
    total += Addr_ToS(bf, sg->val.ptr, flags);
    total += Buff_Bytes(bf, (byte *)">", 1);
    return total;
}

static i64 WrappedUtil_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    Str *s = Str_FromI64(bf->m, sg->val.value);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "Wu\\<^D.$^d.>", args);
    }else{
        return ToS(bf, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedMemCount_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_MEMCOUNT);
    Str *s = Str_MemCount(bf->m, sg->val.value);
    return Buff_Bytes(bf, s->bytes, s->length);
}

static i64 WrappedI64_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    Str *s = Str_FromI64(bf->m, sg->val.value);
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "Wi64\\<^D.$^d.>", args);
    }else{
        return ToS(bf, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI32_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I32);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.i);
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "Wi32\\<^D.$^d.>", args);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else{
        return ToS(bf, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI16_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I16);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.w);
    if(flags & MORE){
        if(sg->objType.of != ZERO){
            Abstract *args[] = {
                (Abstract *)Type_ToStr(bf->m, sg->objType.of),
                (Abstract *)s,
                NULL
            };
            return Fmt(bf, "Wi16:$\\<^D.$^d.>", args);
        }else{
            Abstract *args[] = {
                (Abstract *)s,
                NULL
            };
            return Fmt(bf, "Wi16\\<^D.$^d.>", args);
        }
    }else{
        return ToS(bf, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedI8_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I8);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.b);
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "Wi8\\<^D.$^d.>", args);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else{
        return ToS(bf, (Abstract *)s, 0, flags);
    }
}

static i64 WrappedB_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_BYTE);
    i64 total = 0;
    if((flags & DEBUG)){
        total += Fmt(bf, "Wb<^E.", NULL);
        total += Bytes_Debug(bf, &sg->val.b, &sg->val.b);
        Str *num = Str_FromI64(bf->m, (i64)sg->val.b);
        total += Fmt(bf, "^e.", NULL);
        total += Buff_Bytes(bf, (byte *)"/", 1);
        total += Buff_Bytes(bf, num->bytes, num->length);
        total += Buff_Bytes(bf, (byte *)">", 1);
    }else if(flags & MORE){
        total += Fmt(bf, "(^E.", NULL);
        total += Bytes_Debug(bf, &sg->val.b, &sg->val.b);
        Str *num = Str_FromI64(bf->m, (i64)sg->val.b);
        total += Fmt(bf, "^e.", NULL);
        total += Buff_Bytes(bf, (byte *)"/", 1);
        total += Buff_Bytes(bf, num->bytes, num->length);
        total += Buff_Bytes(bf, (byte *)")", 1);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
    return total;
}

static i64 WrappedMicroTime_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    Str *s = MicroTime_ToStr(bf->m, sg->val.value);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            (Abstract *)s,
            NULL
        };
        return Fmt(bf, "Wt64\\<$ ^D.@^d.>", args);
    }else{
        return ToS(bf, (Abstract *)s, 0, flags);
    }
}

static i64 Abstract_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Fmt(bf, "A\\<$>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static i64 Single_Print(Buff *bf, Abstract *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED);
    if(flags & (DEBUG|MORE)){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Fmt(bf, "Single\\<$>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

status Util_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)WrappedI64_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I32, (void *)WrappedI32_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I16, (void *)WrappedI16_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I8, (void *)WrappedI8_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_BYTE, (void *)WrappedB_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_TIME64, (void *)WrappedMicroTime_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_DO, (void *)WrappedDo_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_PTR, (void *)WrappedPtr_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_FUNC, (void *)WrappedFunc_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_MEMCOUNT, (void *)WrappedMemCount_Print);
    return r;
}
