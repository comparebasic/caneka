#include <external.h>
#include "base_module.h" 

static status Wrapped_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)asIfc(a, TYPE_WRAPPED);
    if(flags & (MORE|DEBUG)){
        void *args[] = {
            Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Fmt(bf, "Wr\\<$>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static status WrappedDo_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_DO);
    if(flags & (DEBUG|MORE)){
        void *args[] = {
            Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            I64_Wrapped(bf->m, (util)sg->val.ptr),
            NULL
        };
        return Fmt(bf, "Wdo\\<$ ^D.*$^d.>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static status WrappedFunc_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_FUNC);
    void *args[] = {
        Type_StateVec(bf->m, sg->type.of, sg->type.state),
        Util_Wrapped(bf->m, (util)sg->val.ptr),
        NULL
    };
    if(flags & MORE){
        return Fmt(bf, "Wfunc\\<$ ^D.$^d.>", args);
    }else{
        return Fmt(bf, "Wfunc\\<$ $>", args);
    }
}

static status WrappedPtr_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    word fl = (DEBUG|MORE);
    void *args[] = {
        Str_CstrRef(bf->m, Type_ToChars(sg->objType.of)),
        NULL
    };
    Fmt(bf, "Wptr<$ ", args);
    if(Lookup_Get(ToStreamLookup, sg->objType.of) != NULL){
        ToS(bf, sg->val.ptr, sg->objType.of, MORE);
#ifdef INSECURE
    }else if(MemBook_Check(sg->val.ptr)){
        Addr_ToS(bf, sg->val.ptr, flags);
#endif
    }else{
        ToS(bf, Util_Wrapped(bf->m, (util)sg->val.ptr), TYPE_WRAPPED_UTIL, ZERO);
    }
    Buff_AddBytes(bf, (byte *)">", 1);
    return SUCCESS;
}

static status WrappedUtil_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    Str *s = Str_FromI64(bf->m, sg->val.value);
    if(flags & MORE){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else if(flags & DEBUG){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "Wu\\<^D.$^d.>", args);
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status WrappedMemCount_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_MEMCOUNT);
    Str *s = Str_MemCount(bf->m, sg->val.value);
    return Buff_AddBytes(bf, s->bytes, s->length);
}

static status WrappedI64_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    Str *s = Str_FromI64(bf->m, sg->val.value);
    if(flags & MORE){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "Wi64\\<^D.$^d.>", args);
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status WrappedU32_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_U32);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.i);
    if(flags & DEBUG){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "Wu32\\<^D.$^d.>", args);
    }else if(flags & MORE){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status WrappedI32_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I32);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.i);
    if(flags & DEBUG){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "Wi32\\<^D.$^d.>", args);
    }else if(flags & MORE){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status WrappedI16_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I16);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.w);
    if(flags & MORE){
        if(sg->objType.of != ZERO){
            void *args[] = {
                Type_ToStr(bf->m, sg->objType.of),
                s,
                NULL
            };
            return Fmt(bf, "Wi16:$\\<^D.$^d.>", args);
        }else{
            void *args[] = {
                s,
                NULL
            };
            return Fmt(bf, "Wi16\\<^D.$^d.>", args);
        }
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status WrappedI8_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I8);
    Str *s = Str_FromI64(bf->m, (i64)sg->val.b);
    if(flags & DEBUG){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "Wi8\\<^D.$^d.>", args);
    }else if(flags & MORE){
        void *args[] = {
            s,
            NULL
        };
        return Fmt(bf, "^D.$^d.", args);
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status WrappedB_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_BYTE);
    if((flags & DEBUG)){
        Fmt(bf, "Wb<^E.", NULL);
        Bytes_Debug(bf, &sg->val.b, &sg->val.b);
        Str *num = Str_FromI64(bf->m, (i64)sg->val.b);
        Fmt(bf, "^e.", NULL);
        Buff_AddBytes(bf, (byte *)"/", 1);
        Buff_AddBytes(bf, num->bytes, num->length);
        Buff_AddBytes(bf, (byte *)">", 1);
    }else if(flags & MORE){
        Fmt(bf, "(^E.", NULL);
        Bytes_Debug(bf, &sg->val.b, &sg->val.b);
        Str *num = Str_FromI64(bf->m, (i64)sg->val.b);
        Fmt(bf, "^e.", NULL);
        Buff_AddBytes(bf, (byte *)"/", 1);
        Buff_AddBytes(bf, num->bytes, num->length);
        Buff_AddBytes(bf, (byte *)")", 1);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
    return SUCCESS;
}

static status WrappedMicroTime_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    Str *s = MicroTime_ToStr(bf->m, sg->val.value);
    if(flags & (DEBUG|MORE)){
        void *args[] = {
            Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            s,
            NULL
        };
        return Fmt(bf, "Wt64\\<$ ^D.@^d.>", args);
    }else{
        return ToS(bf, s, 0, flags);
    }
}

static status Abstract_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_UTIL);
    if(flags & (DEBUG|MORE)){
        void *args[] = {
            Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
            NULL
        };
        return Fmt(bf, "A\\<$>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static status Single_Print(Buff *bf, void *a, cls type, word flags){
    Single *sg = (Single *)as(a, TYPE_WRAPPED);
    if(flags & (DEBUG|MORE)){
        void *args[] = {
            Str_CstrRef(bf->m, Type_ToChars(sg->type.of)),
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
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_U32, (void *)WrappedU32_Print);
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
