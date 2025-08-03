#include <external.h>
#include <caneka.h>

static char *baseTypeStrings[] = {
    "_TYPE_ZERO",
    "TYPE_UTIL",
    "TYPE_I64",
    "TYPE_U32",
    "TYPE_I32",
    "TYPE_WORD",
    "TYPE_I16",
    "TYPE_BYTE",
    "TYPE_I8",
    "_TYPE_RAW_END",
    "_TYPE_WRAPPED_START",
    "TYPE_WRAPPED_PTR",
    "TYPE_WRAPPED_FUNC",
    "TYPE_WRAPPED_DO",
    "TYPE_WRAPPED_MEMCOUNT",
    "TYPE_WRAPPED_TIME64",
    "TYPE_WRAPPED",
    "TYPE_WRAPPED_UTIL",
    "TYPE_WRAPPED_I64",
    "TYPE_WRAPPED_U32",
    "TYPE_WRAPPED_I32",
    "TYPE_WRAPPED_WORD",
    "TYPE_WRAPPED_I16",
    "TYPE_WRAPPED_BYTE",
    "TYPE_WRAPPED_I8",
    "TYPE_WRAPPED_BOOL",
    "TYPE_WRAPPED_CSTR",
    "_TYPE_WRAPPED_END",
    "TYPE_UNKNOWN",
    "TYPE_ABSTRACT",
    "TYPE_BUILDCTX",
    "TYPE_RANGE",
    "TYPE_BOOK",
    "TYPE_MEMCTX",
    "TYPE_EXTERNAL_FREE",
    "TYPE_MEMSLAB",
    "TYPE_MAKER",
    "TYPE_COMPRESULT",
    "TYPE_COMP",
    "TYPE_CSTR",
    "TYPE_STR",
    "TYPE_COORD",
    "TYPE_STRLIT",
    "TYPE_STRVEC",
    "TYPE_STREAM",
    "TYPE_STREAM_TASK",
    "TYPE_CURSOR",
    "TYPE_TESTSUITE",
    "TYPE_SPAN",
    "TYPE_TABLE",
    "TYPE_ARRAY",
    "TYPE_COORDS",
    "TYPE_SLAB",
    "TYPE_HASHED",
    "TYPE_LOOKUP",
    "TYPE_ITER",
    "TYPE_GUARD",
    "TYPE_HKEY",
    "TYPE_PROCDETS",
    "TYPE_DEBUG_STACK",
    "TYPE_DEBUG_STACK_ENTRY",
    "_TYPE_BASE_END",
    NULL
};

char *Type_ToChars(cls type){
    Iter it;
    Iter_Init(&it, TypeStringRanges->values);
    word offset = TypeStringRanges->offset;
    char **typeStrings = NULL;
    i32 idx = 0;
    while((Iter_Next(&it) & END) == 0){
        if(it.idx > type){
            break;
        }else{
            typeStrings = (char **)Iter_Get(&it);
            idx = it.idx;
        }
    }
    if(typeStrings != NULL){
        return typeStrings[type-idx];
    }
    return "TYPE_unknown";
}

Str *Type_ToStr(MemCh *m, cls type){
    char *cstr = Type_ToChars(type);
    return Str_CstrRef(m, cstr);
}

status DebugTypeStrings_Init(MemCh *m){
    return Lookup_Add(m, TypeStringRanges, _TYPE_ZERO, (void *)baseTypeStrings);
}
