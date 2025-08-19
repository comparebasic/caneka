#include <external.h>
#include <caneka.h>

Lookup *SizeLookup = NULL;

static status setSizeLookup(MemCh *m, Lookup *lk){
    status r = READY;

    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_FUNC, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_DO, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_UTIL, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_MEMCOUNT, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I64, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I32, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I16, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I8, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_TIME64, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_BOOL, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_PTR, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_CSTR, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_BOOK, (util)sizeof(MemBook));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMCTX, (util)sizeof(MemCh));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMSLAB, (util)sizeof(MemPage));
    r |= Lookup_AddRaw(m, lk, TYPE_STR, (util)sizeof(Str));
    r |= Lookup_AddRaw(m, lk, TYPE_STRVEC, (util)sizeof(StrVec));
    r |= Lookup_AddRaw(m, lk, TYPE_STREAM, (util)sizeof(Stream));
    r |= Lookup_AddRaw(m, lk, TYPE_STREAM_TASK, (util)sizeof(StreamTask));
    r |= Lookup_AddRaw(m, lk, TYPE_FMT_LINE, (util)sizeof(FmtLine));
    r |= Lookup_AddRaw(m, lk, TYPE_CURSOR, (util)sizeof(Cursor));
    r |= Lookup_AddRaw(m, lk, TYPE_TESTSUITE, (util)sizeof(TestSet));
    r |= Lookup_AddRaw(m, lk, TYPE_SNIP, (util)sizeof(Snip));
    r |= Lookup_AddRaw(m, lk, TYPE_SNIPSPAN, (util)sizeof(Span));
    r |= Lookup_AddRaw(m, lk, TYPE_SPAN, (util)sizeof(Span));
    r |= Lookup_AddRaw(m, lk, TYPE_TABLE, (util)sizeof(Table));
    r |= Lookup_AddRaw(m, lk, TYPE_HKEY, (util)sizeof(HKey));
    r |= Lookup_AddRaw(m, lk, TYPE_HASHED, (util)sizeof(Hashed));
    r |= Lookup_AddRaw(m, lk, TYPE_ITER, (util)sizeof(Iter));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED, (util)sizeof(Single));

    return r;
}

void Type_SetFlag(Abstract *a, word flags){
    a->type.state = (a->type.state & NORMAL_FLAGS) | flags;
}

cls Ifc_Get(cls inst){
    if(inst == TYPE_SPAN || inst == TYPE_TABLE){
        return TYPE_SPAN;
    }else if(inst >= TYPE_WRAPPED && inst <= _TYPE_WRAPPED_END){
        return TYPE_WRAPPED;
    }

    return inst;
}

boolean Ifc_Match(cls inst, cls ifc){
    if(inst == ifc){
        return TRUE;
    }

    if(ifc == TYPE_MEMCTX){
        return inst == TYPE_SPAN || inst == TYPE_REQ || inst == TYPE_SERVECTX 
            || inst == TYPE_ROEBLING;
    }else if(ifc == TYPE_WRAPPED){
        return (inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL ||
            inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR ||
            inst == TYPE_WRAPPED_I64 || inst == TYPE_WRAPPED_I32 ||
            inst == TYPE_WRAPPED_I16 || inst == TYPE_WRAPPED_I8);
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_TABLE); 
    }

    return FALSE;
}

status Ifc_Init(MemCh *m){
    if(SizeLookup == NULL){
        SizeLookup = Lookup_Make(m, _TYPE_ZERO);
        return setSizeLookup(m, SizeLookup);
    }
    return NOOP;
}
