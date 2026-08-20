#include <external.h>
#include <caneka.h>

static char *algoTypeStrings[] = {
    "_TYPE_ALGO_START",
    "TYPE_NGRAM",
    "TYPE_NGRAM_REC",
    "_TYPE_ALGO_END",
    NULL
};

status AlgoTypeStrings_Init(MemCh *m){
    status r = READY;
    r |=  Lookup_Add(m, TypeStringRanges, _TYPE_ALGO_START, (void *)algoTypeStrings);
    return r;
}
