#include <external.h>
#include "base_module.h"

typedef struct sztype {
    cls typeOf;
    ssize_t sz;
} szType;

static void *Str_Conv(MemCh *m, void *_a, IfcMap *imap){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_STR){
        return (Str *)a;
    }else if(a->type.of == TYPE_STRVEC){
        return StrVec_Str(m, (StrVec *)a);
    }
    return NULL;
}

static void *StrVec_Conv(MemCh *m, void *_a, IfcMap *imap){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_STRVEC){
        return (StrVec *)a;
    }else if(a->type.of == TYPE_STR){
        return StrVec_From(m, (Str *)a);
    }
    return NULL;
}

static void *CStr_Conv(MemCh *m, void *_a, IfcMap *imap){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_STRVEC){
       a = (Abstract *)StrVec_Str(m, (StrVec *)a); 
    }

    if(a->type.of != TYPE_STR){
        return NULL;
    }

    return Str_Cstr(m, (Str *)a);
}

void Base_IfcInit(MemCh *m){
    Lookup_Add(m, IfcLookup, TYPE_STR, 
        IfcMap_Make(m,
            TYPE_STR,
            ZERO,
            TYPE_STRVEC - TYPE_STR,
            sizeof(Str),
            Str_Conv));
    Lookup_Add(m, IfcLookup, TYPE_STRVEC,
        IfcMap_Make(m,
            TYPE_STRVEC,
            ZERO,
            TYPE_STRVEC -TYPE_STR,
            sizeof(StrVec),
            StrVec_Conv));

    Lookup_Add(m, IfcLookup, TYPE_CSTR,
        IfcMap_Make(m,
            TYPE_CSTR,
            ZERO,
            TYPE_STRVEC - TYPE_CSTR,
            -1,
            CStr_Conv));

    szType nonPolyTypes[] = {
        {TYPE_WRAPPED, sizeof(Single)},
        {TYPE_WRAPPED_UTIL, sizeof(Single)},
        {TYPE_WRAPPED_I64, sizeof(Single)},
        {TYPE_WRAPPED_U32, sizeof(Single)},
        {TYPE_WRAPPED_I32, sizeof(Single)},
        {TYPE_WRAPPED_WORD, sizeof(Single)},
        {TYPE_WRAPPED_I16, sizeof(Single)},
        {TYPE_WRAPPED_BYTE, sizeof(Single)},
        {TYPE_WRAPPED_I8, sizeof(Single)},
        {TYPE_WRAPPED_CSTR, sizeof(Single)},
        {TYPE_BUFF, sizeof(Buff)},
        {TYPE_SPAN, sizeof(Span)},
        {TYPE_TABLE, sizeof(Table)},
        {TYPE_MEMCTX, sizeof(MemCh)},
        {TYPE_MEMSLAB, sizeof(MemPage)},
        {TYPE_MEM_IDENT, sizeof(MemIdent)},
        {TYPE_DEBUG_STACK_ENTRY, sizeof(StackEntry)},
        {TYPE_HASHED, sizeof(Hashed)},
        {0, 0}
    };

    szType *t = nonPolyTypes;
    while(t->sz != 0){
        Lookup_Add(m, IfcLookup, t->typeOf,
            IfcMap_Make(m, t->typeOf, ZERO, ZERO, t->sz, NULL));
        t++;
    }
    
    /*
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_FUNC, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_DO, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_UTIL, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_MEMCOUNT, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_I64, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_I32, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_I16, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_I8, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_BOOL, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_PTR, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED_CSTR, (i64)sizeof(Single));
    Lookup_Add(m, IfcLookup, TYPE_BOOK, (i64)sizeof(MemBook));
    Lookup_Add(m, IfcLookup, TYPE_MEMCTX, (i64)sizeof(MemCh));
    Lookup_Add(m, IfcLookup, TYPE_MEMSLAB, (i64)sizeof(MemPage));
    Lookup_Add(m, IfcLookup, TYPE_STR, (i64)sizeof(Str));
    Lookup_Add(m, IfcLookup, TYPE_STRVEC, (i64)sizeof(StrVec));
    Lookup_Add(m, IfcLookup, TYPE_FMT_LINE, (i64)sizeof(FmtLine));
    Lookup_Add(m, IfcLookup, TYPE_CURSOR, (i64)sizeof(Cursor));
    Lookup_Add(m, IfcLookup, TYPE_SPAN, (i64)sizeof(Span));
    Lookup_Add(m, IfcLookup, TYPE_TABLE, (i64)sizeof(Table));
    Lookup_Add(m, IfcLookup, TYPE_LOOKUP, (i64)sizeof(Lookup));
    Lookup_Add(m, IfcLookup, TYPE_HKEY, (i64)sizeof(HKey));
    Lookup_Add(m, IfcLookup, TYPE_HASHED, (i64)sizeof(Hashed));
    Lookup_Add(m, IfcLookup, TYPE_ITER, (i64)sizeof(Iter));
    Lookup_Add(m, IfcLookup, TYPE_WRAPPED, (i64)sizeof(Single));
    */
}
