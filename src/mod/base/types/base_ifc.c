#include <external.h>
#include "base_module.h"

static void *Str_Conv(MemCh *m, void *_a, IfcMap *imap){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_STRVEC){
        return StrVec_Str(m, a);
    }
    return NULL;
}

static void *StrVec_Conv(MemCh *m, void *_a, IfcMap *imap){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_STR){
        return StrVec_From(m, a);
    }
    return NULL;
}

static void *CStr_Conv(MemCh *m, void *_a, IfcMap *imap){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_STRVEC){
       a = StrVec_Str(m, a); 
    }

    if(a->type.of != TYPE_STR){
        return NULL;
    }

    return Str_Cstr(m, a);
}

void Base_IfcInit(MemCh *m){
    Lookup_Add(m, IfcLookup, TYPE_STR, 
        IfcMap_Make(m,
            TYPE_STR,
            TYPE_STRVEC - TYPE_STR,
            TYPE_STRVEC - TYPE_STR,
            sizeof(Str),
            Str_Conv));
    Lookup_Add(m, IfcLookup, TYPE_STRVEC,
        IfcMap_Make(m,
            TYPE_STR,
            TYPE_STR - TYPE_STRVEC,
            TYPE_STR - TYPE_STRVEC,
            sizeof(StrVec),
            StrVec_Conv));
    Lookup_Add(m, IfcLookup, TYPE_CSTR,
        IfcMap_Make(m,
            TYPE_STR,
            TYPE_CSTR - TYPE_STR,
            TYPE_CSTR - TYPE_STRVEC,
            -1,
            CStr_Conv));
    
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
