#include <external.h>
#include <caneka.h>

InstApi *InstApi_From(MemCh *m, Inst *inst){
    IterApi *api = Lookup_Get(IterApiLookup, inst->type.of);
    if(api == NULL){
        api = Lookup_Get(IterApiLookup, TYPE_ITER);
    }
    return InstApi_Make(m, inst, Iter_Make(m, NULL), api);
}

InstApi *InstApi_Make(MemCh *m, Inst *inst, Iter *it, IterApi *api){
    InstApi *itn = MemCh_AllocOf(m, sizeof(InstApi), TYPE_INST_ITER);
    itn->type.of = TYPE_INST_ITER;
    itn->objType.of = inst->type.of;
    itn->inst = inst;
    itn->it = it;
    itn->api = api;
    return itn;
}
