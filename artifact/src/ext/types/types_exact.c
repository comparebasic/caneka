#include <external.h>
#include <caneka.h>

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    return a->type.of == b->type.of && a->type.state == b->type.state && 
        a->objType.state == b->objType.state &&
        (a->path == b->path || Exact((Abstract *)a->path, (Abstract *)b->path)) &&
        (a->key == b->key || Exact((Abstract *)a->key, (Abstract *)b->key)) &&
        (a->idx == b->idx);
}

status Types_ExactInit(MemCh *m){
    status r = READY;
    r |= Lookup_Add(m, ExactLookup, TYPE_FETCHER, (void *)Fetcher_Exact); 
    return r;
}
