#include <external.h>
#include <caneka.h>

Lookup *InstIterFuncLookup = NULL;

InstIter *InstIter_From(MemCh *m, Inst *inst){
    NextFunc func = Lookup_Get(InstIterFuncLookup, inst->type.of);
    if(func == NULL){
        func = Iter_Next;
    }
    return InstIter_Make(m, inst, func);
}

InstIter *InstIter_Make(MemCh *m, Inst *inst, NextFunc func){
    InstIter *itn = MemCh_AllocOf(m, sizeof(InstIter), TYPE_INST_ITER);
    itn->type.of = TYPE_INST_ITER;
    itn->objType.of = inst->type.of;
    itn->inst = inst;
    itn->func = func;
    return itn;
}

status InstIter_Init(MemCh *m){
    status r = READY;
    if(InstIterFuncLookup == NULL){
        InstIterFuncLookup = Lookup_Make(m, TYPE_INSTANCE);
        r |= SUCCESS;
    }
    return r;
}
