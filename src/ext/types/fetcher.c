#include <external.h>
#include <caneka.h>

void *Fetch(MemCh *m, Fetcher *fch, void *_value, void *source){
    Abstract *value = (Abstract *)_value;
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
        if((it.type.state & LAST) == 0 && value->type.of == TYPE_HASHED){
            value = ((Hashed *)value)->value;
        }

        tg->type.state |= (fch->type.state & PROCESSING);
        value = Fetch_Target(m, tg, value, source);
    }

    if(1 || fch->type.state & DEBUG){
        void *args[] = {
            fch,
            value,
            NULL,
        };
        Out("^c.after Fetch & value = ^y.@^0.\n", args);
    }

    if(it.type.state & END){
        return value;
    }else if((fch->type.state & FETCHER_IF) == 0){
        void *args[] = {
            fch,
            orig,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Fetch @ value not found on @\n", args);
        return NULL;
    }
    return NULL;
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->val.targets = Span_Make(m);
    return fch;
}
