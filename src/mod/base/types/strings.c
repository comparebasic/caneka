/* = Base.types.strings
 *
 * Debug strings for the type system of the *base* module
 *
 */
#include <external.h>
#include "base_module.h"

static char *baseTypeStrings[] = {
    "_TYPE_ZERO",
    "UTIL",
    "I64",
    "U32",
    "I32",
    "WORD",
    "I16",
    "BYTE",
    "I8",
    "TIMESPEC",
    "_TYPE_RAW_END",
    "_TYPE_WRAPPED_START",
    "WRAPPED_PTR",
    "WRAPPED_PTR_TIMESPEC",
    "WRAPPED_FUNC",
    "WRAPPED_DO",
    "WRAPPED",
    "WRAPPED_UTIL",
    "WRAPPED_I64",
    "WRAPPED_U32",
    "WRAPPED_I32",
    "WRAPPED_WORD",
    "WRAPPED_I16",
    "WRAPPED_BYTE",
    "WRAPPED_I8",
    "WRAPPED_BOOL",
    "WRAPPED_CSTR",
    "WRAPPED_MEMCOUNT",
    "_TYPE_WRAPPED_END",
    "UNKNOWN",
    "_TYPE_RANGE_TYPE_START",
    "BYTES_POINTER",
    "POINTER_ARRAY",
    "RANGE_ARRAY",
    "_TYPE_RANGE_TYPE_END",
    "_TYPE_ABSTRACT_BEGIN",
    "ABSTRACT",
    "ERROR_MSG",
    "BUILDCTX",
    "RANGE",
    "BOOK",
    "BOOK_STATS",
    "MEMCTX",
    "MEM_ITER",
    "MEM_IDENT",
    "STASH_ITEM",
    "MAP",
    "IFC_MAP",
    "EXTERNAL_FREE",
    "MEMSLAB",
    "MAKER",
    "COMPRESULT",
    "NONE",
    "COMP",
    "CSTR",
    "STR",
    "STRVEC",
    "HISTO",
    "COORD",
    "STREAM",
    "STREAM_TASK",
    "BUFF",
    "FILE",
    "POLLFD_PTR",
    "CURSOR",
    "TESTSUITE",
    "SPAN",
    "TABLE",
    "ARRAY",
    "CSTR_ARRAY",
    "COORDS",
    "SLAB",
    "HASHED",
    "LOOKUP",
    "ITER",
    "ITINERARY",
    "SEEL",
    "GUARD",
    "HKEY",
    "PROCDETS",
    "DAEMON",
    "DIR_SELECTOR",
    "ACCESS",
    "APPROXTIME",
    "DEBUG_STACK",
    "DEBUG_STACK_ENTRY",
    "CLI_STATUS",
    "CLI_ARGS",
    "FMT_LINE",
    "PATMATCH",
    "PATCHAR",
    "PATCHARDEF",
    "SNIP",
    "SNIPSPAN",
    "ROEBLING",
    "_JSON_START",
    "JSON_START",
    "JSON_LEAD",
    "JSON_INDENT",
    "JSON_OUTDENT",
    "JSON_ARR",
    "JSON_ARR_INDENT",
    "JSON_ARR_OUTDENT",
    "JSON_STRING",
    "JSON_NUMBER",
    "JSON_KEY",
    "JSON_VALUE",
    "JSON_SEP",
    "JSON_KEY_SEP",
    "JSON_KEY_VALUE",
    "JSON_KEY_VALUE_SEP",
    "JSON_COMMA_SEP",
    "JSON_END",
    "_JSON_END",
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
        if(type < 1 << 15){
            return typeStrings[type-idx];
        }else{
            return "TooBigTypeNumber";
        }
    }
    return "unknown";
}

static char *instStrings[] = {
    "INSTANCE",
    "NODE",
    "_BASE_INST_END",
    NULL
};


Str *Type_ToStr(MemCh *m, cls type){
    char *cstr = Type_ToChars(type);
    return Str_CstrRef(m, cstr);
}

status DebugTypeStrings_Init(MemCh *m){
    status r = READY;
    r |=  Lookup_Add(m, TypeStringRanges, _TYPE_ZERO, (void *)baseTypeStrings);
    r |=  Lookup_Add(m, TypeStringRanges, TYPE_INSTANCE, (void *)instStrings);
    return r;
}
