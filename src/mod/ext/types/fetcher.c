#include <external.h>
#include <caneka.h>

void Fetch(MemCh *m, Fetcher *fch, void *_value, void *source){
    Debug_Push(m, fch);
    fch->value = NULL;

    Abstract *value = (Abstract *)_value;
    if(value != NULL){
        if(value->type.of == TYPE_ITER){
            Iter *it = (Iter *)value;
            Debug_SetRef(m, it->p);
        }else{
            Debug_SetRef(m, value);
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

    Iter_Restart(&fch->targets);

    FetchTarget *tg = NULL;
    while(value != NULL && (Iter_Next(&fch->targets) & END) == 0){
        tg = (FetchTarget *)Iter_Get(&fch->targets);
        if(tg->type.of != TYPE_FETCH_TARGET){
            break;
        }
        if(value->type.of == TYPE_HASHED && 
                (tg->type.state & FETCH_TARGET_ATT) == 0){
            value = ((Hashed *)value)->value;
        }

        tg->type.state |= (fch->type.state & PROCESSING);
        if((tg->type.state & FETCH_TARGET_ITER) && fch->api == NULL){
            cls typeOf = value->type.of;
            IterApi *api = NULL;
            if(typeOf == TYPE_ITER && ((Iter *)value)->itin != NULL){
                typeOf = ((Iter *)value)->itin->objType.of;
                api = Lookup_Get(IterApiLookup, typeOf);
            }
            if(api == NULL){
                api = BaseIterApi;
            }

            fch->api = api;
        }
        value = Fetch_Target(m, tg, value, source);
        if(fch->type.state & DEBUG){
            void *args[] = {
                value,
                tg,
                orig,
                NULL,
            };
            Out("^p.    Fetch value = ^y.@ from @/@^0.\n", args);
        }
    }

    if(fch->type.state & DEBUG){
        void *args[] = {
            fch,
            value,
            NULL,
        };
        Out("^c.after Fetch & value = ^y.@^0.\n", args);
    }

    if(fch->targets.type.state & END){
        if(value->type.of == TYPE_HASHED && 
                (tg->type.state & FETCH_TARGET_ATT) == 0){
            value = ((Hashed *)value)->value;
        }
    }

    if(value != orig){
        fch->value = value;
    }
    Debug_Pop(m);
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    Iter_Init(&fch->targets,Span_Make(m));
    return fch;
}
