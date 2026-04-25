#include <external.h>
#include <caneka.h>

static char *interTypeStrings[] = {
    "_TYPE_INTER_START",
    "_TYPE_INTER_END",
    NULL
};

static char *interTypeObjectStrings[] = {
    "_TYPE_INTER_OBJ_START",
    "_TYPE_INTER_OBJ_END",
    NULL
};

status InterTypeStrings_Init(MemCh *m){
    status r = READY;
    r |= Lookup_Add(m, TypeStringRanges, _TYPE_INTER_START, (void *)interTypeStrings);
    r |= Lookup_Add(m, TypeStringRanges, _TYPE_INTER_OBJ_START, (void *)interTypeObjectStrings);
    return r;
}
