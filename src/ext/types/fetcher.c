#include <external.h>
#include <caneka.h>

void *Fetch_FromOffset(MemCh *m, void *a, i16 offset, cls typeOf){
    void *value = NULL;
    if(typeOf == ZERO || typeOf > _TYPE_RAW_END){
        void **ptr = (void **)(((void *)a)+offset);
        value = *ptr; 
    }else{
        if(typeOf == TYPE_UTIL){
            util *ptr = ((void *)a)+offset;
            value = Util_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I32){
            i32 *ptr = ((void *)a)+offset;
            value = I32_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I64){
            i64 *ptr = ((void *)a)+offset;
            value = I64_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I16){
            i16 *ptr = ((void *)a)+offset;
            value = I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_I8){
            i8 *ptr = ((void *)a)+offset;
            value = I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_BYTE){
            byte *ptr = ((void *)a)+offset;
            value = B_Wrapped(m, *ptr, ZERO, ZERO); 
        }
    }
    return value;
}

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
        if(value->type.of == TYPE_HASHED && ((fch->type.state & FETCHER_COMMAND) == 0)){
            printf("value finagle\n");
            fflush(stdout);
            value = ((Hashed *)value)->value;
        }

        void *args[2];
        args[0] = Type_StateVec(m, tg->type.of, tg->type.state);
        args[1] = NULL;
        Out("^c.Fetch_Target calling @^0\n", args);

        tg->type.state |= (fch->type.state & PROCESSING);
        value = Fetch_Target(m, tg, value, source);
    }

    if(fch->type.state & DEBUG){
        void *args[] = {
            fch,
            value,
            NULL,
        };
        Out("^c.after Fetch & from @^0.\n", args);
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
