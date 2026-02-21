Lookup *EmptyLookup = NULL;

boolean Empty(void *_a){
    if(_a == NULL){
        return TRUE;
    }
    Abstract *a = (Abstract *)_a;
    Func func = Lookup_Get(EmptyLookup, a->type.of);
    if(func != NULL){
        return func(a);
    }
    return FALSE;
}

status Empty_Init(MemCh *m){
    status r = READY;
    if(EmptyLookup == NULL){
        EmptyLookup = Lookup_Make(m, ZERO);
    }
    return r;
}
