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
    r |= Lookup_AddRaw(m, lk, TYPE_BOOK, (i64)sizeof(MemBook));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMCTX, (i64)sizeof(MemCh));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMSLAB, (i64)sizeof(MemPage));
    r |= Lookup_AddRaw(m, lk, TYPE_STR, (i64)sizeof(Str));
    r |= Lookup_AddRaw(m, lk, TYPE_STRVEC, (i64)sizeof(StrVec));
    r |= Lookup_AddRaw(m, lk, TYPE_STREAM, (i64)sizeof(Stream));
    r |= Lookup_AddRaw(m, lk, TYPE_STREAM_TASK, (i64)sizeof(StreamTask));
    r |= Lookup_AddRaw(m, lk, TYPE_FMT_LINE, (i64)sizeof(FmtLine));
    r |= Lookup_AddRaw(m, lk, TYPE_CURSOR, (i64)sizeof(Cursor));
    r |= Lookup_AddRaw(m, lk, TYPE_TESTSUITE, (i64)sizeof(TestSet));
    r |= Lookup_AddRaw(m, lk, TYPE_SPAN, (i64)sizeof(Span));
    r |= Lookup_AddRaw(m, lk, TYPE_TABLE, (i64)sizeof(Table));
    r |= Lookup_AddRaw(m, lk, TYPE_LOOKUP, (i64)sizeof(Lookup));
    r |= Lookup_AddRaw(m, lk, TYPE_HKEY, (i64)sizeof(HKey));
    r |= Lookup_AddRaw(m, lk, TYPE_HASHED, (i64)sizeof(Hashed));
    r |= Lookup_AddRaw(m, lk, TYPE_ITER, (i64)sizeof(Iter));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED, (i64)sizeof(Single));

    return r;
}

Abstract *_as(char *func, char *file, i32 line, Abstract *x, cls type){
    if(x == NULL){
        Fatal(func, file, line, "Cast from NULL", NULL);
    }else {
        if(x->type.of != type){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(func, file, line, "Cast from Abstract mismatched type expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

Abstract *_asIfc(char *func, char *file, i32 line, Abstract *x, cls type){
    if(x == NULL){
        Fatal(func, file, line, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of, type)){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(func, file, line, "Cast from Abstract mismatched interface, expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
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

    if(ifc == TYPE_WRAPPED){
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
