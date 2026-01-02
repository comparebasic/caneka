#include <external.h>
#include <caneka.h>

Lookup *IterApiLookup = NULL;

IterApi *IterApi_Make(MemCh *m, IterFunc next, IterFunc prev, IterGetFunc get){
    IterApi *api = MemCh_AllocOf(m, sizeof(IterApi), TYPE_ITER_API);
    api->type.of = TYPE_ITER_API;
    api->next = next;
    api->prev = prev;
    api->get = get;
    return api;
}

status IterApi_Init(MemCh *m){
    status r = READY;
    if(IterApiLookup == NULL){
        IterApiLookup = Lookup_Make(m, TYPE_INSTANCE);
        r |= SUCCESS;
    }
    return r;
}
