#include <external.h>
#include "base_module.h"

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
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_BOOL, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_PTR, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_CSTR, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_BOOK, (i64)sizeof(MemBook));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMCTX, (i64)sizeof(MemCh));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMSLAB, (i64)sizeof(MemPage));
    r |= Lookup_AddRaw(m, lk, TYPE_STR, (i64)sizeof(Str));
    r |= Lookup_AddRaw(m, lk, TYPE_STRVEC, (i64)sizeof(StrVec));
    r |= Lookup_AddRaw(m, lk, TYPE_FMT_LINE, (i64)sizeof(FmtLine));
    r |= Lookup_AddRaw(m, lk, TYPE_CURSOR, (i64)sizeof(Cursor));
    r |= Lookup_AddRaw(m, lk, TYPE_SPAN, (i64)sizeof(Span));
    r |= Lookup_AddRaw(m, lk, TYPE_TABLE, (i64)sizeof(Table));
    r |= Lookup_AddRaw(m, lk, TYPE_LOOKUP, (i64)sizeof(Lookup));
    r |= Lookup_AddRaw(m, lk, TYPE_HKEY, (i64)sizeof(HKey));
    r |= Lookup_AddRaw(m, lk, TYPE_HASHED, (i64)sizeof(Hashed));
    r |= Lookup_AddRaw(m, lk, TYPE_ITER, (i64)sizeof(Iter));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED, (i64)sizeof(Single));

    return r;
}

void *_asError(char *func, char *file, i32 line, void *x, cls type){
    Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
        "Error anonymous from _asError", NULL);
    return x;
}

void *_as(char *func, char *file, i32 line, void *_x, cls type){
    Abstract *x = (Abstract *)_x;
    if(x == NULL){
        Error(ErrStream->m, func, file, line, "Cast from NULL", NULL);
    }else {
        if(x->type.of != type){
            void *args[] = {
                Str_CstrRef(ErrStream->m, Type_ToChars(type)),
                Str_CstrRef(ErrStream->m, Type_ToChars(x->type.of)),
                NULL
            };
            Error(ErrStream->m, func, file, line, "Cast from Abstract mismatched type expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

void *_asIfc(char *func, char *file, i32 line, void *_x, cls type){
    Abstract *x = (Abstract *)_x;
    if(x == NULL){
        Error(ErrStream->m, func, file, line, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of, type)){
            void *args[] = {
                Str_CstrRef(ErrStream->m, Type_ToChars(type)),
                Str_CstrRef(ErrStream->m, Type_ToChars(x->type.of)),
                NULL
            };
            Error(ErrStream->m, func, file, line, "Cast from Abstract mismatched interface, expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

void Type_SetFlag(void *_a, word flags){
    Abstract *a = (Abstract *)_a;
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
