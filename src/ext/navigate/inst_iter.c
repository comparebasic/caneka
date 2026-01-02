#include <external.h>
#include <caneka.h>

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
