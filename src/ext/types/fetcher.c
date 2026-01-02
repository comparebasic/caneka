#include <external.h>
#include <caneka.h>

void *Fetch(MemCh *m, Fetcher *fch, void *_value, void *source){
    DebugStack_Push(fch, fch->type.of);
    Abstract *value = (Abstract *)_value;
    if(value != NULL){
        if(value->type.of == TYPE_ITER){
            Iter *it = (Iter *)value;
            DebugStack_SetRef(it->p, it->p->type.of);
        }else{
            DebugStack_SetRef(value, value->type.of);
        }
    }
    Abstract *orig = value;
    if(fch->type.state & DEBUG){
        void *args[] = {
            fch,
            value,
            NULL,
        };
        Out("^c.Fetch & from @^0.\n", args);
    }

    Iter it;
    Iter_Init(&it, fch->val.targets);
    while(value != NULL && (Iter_Next(&it) & END) == 0){
        FetchTarget *tg = (FetchTarget *)Iter_Get(&it);
        if(value->type.of == TYPE_HASHED && 
                (tg->type.state & FETCH_TARGET_ATT) == 0){
            value = ((Hashed *)value)->value;
        }

        tg->type.state |= (fch->type.state & PROCESSING);
        if((tg->type.state & FETCH_TARGET_ITER) && fch->api == NULL){
            IterApi *api = Lookup_Get(IterApiLookup, value->type.of);
            if(api == NULL){
                api = BaseIterApi;
            }
            fch->api = api;
        }
        value = Fetch_Target(m, tg, value, source);
    }

    if(fch->type.state & DEBUG){
        void *args[] = {
            fch,
            value,
            NULL,
        };
        Out("^c.after Fetch & value = ^y.@^0.\n", args);
    }

    if(it.type.state & END){
        DebugStack_Pop();
        return value;
    }else if((fch->type.state & FETCHER_IF) == 0){
        void *args[] = {
            fch,
            orig,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Fetch @ value not found on @\n", args);
        DebugStack_Pop();
        return NULL;
    }
    DebugStack_Pop();
    return NULL;
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->val.targets = Span_Make(m);
    return fch;
}
