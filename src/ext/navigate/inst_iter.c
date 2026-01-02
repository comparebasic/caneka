#include <external.h>
#include <caneka.h>

Lookup *IterApiLookup = NULL;

InstIter *InstIter_From(MemCh *m, Inst *inst){
    IterApi *api = Lookup_Get(IterApiLookup, inst->type.of);
    if(api == NULL){
        api = Lookup_Get(IterApiLookup, TYPE_ITER);
    }
    return InstIter_Make(m, inst, Iter_Make(m, NULL), api);
}

InstIter *InstIter_Make(MemCh *m, Inst *inst, Iter *it, IterApi *api){
    InstIter *itn = MemCh_AllocOf(m, sizeof(InstIter), TYPE_INST_ITER);
    itn->type.of = TYPE_INST_ITER;
    itn->objType.of = inst->type.of;
    itn->inst = inst;
    itn->it = it;
    itn->api = api;
    return itn;
}

IterApi *IterApi_Make(MemCh *m, IterFunc next, IterFunc prev, IterGetFunc get){
    IterApi *api = MemCh_AllocOf(m, sizeof(IterApi), TYPE_ITER_API);
    api->type.of = TYPE_ITER_API;
    api->next = next;
    api->prev = prev;
    api->get = get;
    return api;
}

status InstIter_Init(MemCh *m){
    status r = READY;
    if(IterApiLookup == NULL){
        IterApiLookup = Lookup_Make(m, TYPE_INSTANCE);
        r |= SUCCESS;
    }
    return r;
}
