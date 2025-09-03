#include <external.h>
#include <caneka.h>

static char *strings[] = {
    "_TYPE_WWW_START",
    "WwwNav",
    "WwwPage",
    "_TYPE_WWW_END",
    NULL
};

status WwwStrings_Init(MemCh *m){
    return Lookup_Add(m, TypeStringRanges, _TYPE_WWW_START, (void *)strings);
}
