#include <external.h>
#include <caneka.h>

Abstract *Fetch_FromOffset(MemCh *m, Abstract *a, i16 offset, cls typeOf){
    Abstract *value = NULL;
    if(typeOf == ZERO || typeOf > _TYPE_RAW_END){
        void **ptr = (void **)(((void *)a)+offset);
        value = *ptr; 
    }else{
        if(typeOf == TYPE_UTIL){
            util *ptr = ((void *)a)+offset;
            value = (Abstract *)Util_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I32){
            i32 *ptr = ((void *)a)+offset;
            value = (Abstract *)I32_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I64){
            i64 *ptr = ((void *)a)+offset;
            value = (Abstract *)I64_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I16){
            i16 *ptr = ((void *)a)+offset;
            value = (Abstract *)I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_I8){
            i8 *ptr = ((void *)a)+offset;
            value = (Abstract *)I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_BYTE){
            byte *ptr = ((void *)a)+offset;
            value = (Abstract *)B_Wrapped(m, *ptr, ZERO, ZERO); 
        }
    }
    return value;
}

Abstract *Fetch(MemCh *m, Fetcher *fch, Abstract *value, Abstract *source){
    Abstract *orig = value;
    if(fch->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)fch,
            (Abstract *)value,
            NULL,
        };
        Out("^c.Fetch & from @^0.\n", args);
    }
    Iter it;
    Iter_Init(&it, fch->val.targets);
    while(value != NULL && (Iter_Next(&it) & END) == 0){
        FetchTarget *tg = (FetchTarget *)Iter_Get(&it);
        if(value->type.of == TYPE_HASHED && (fch->type.state & (FETCHER_COMMAND)) == 0){
            value = ((Hashed *)value)->value;
        }
        value = Fetch_Target(m, tg, value, source);
    }

    if(it.type.state & END){
        return value;
    }else{
        Abstract *args[] = {
            (Abstract *)fch,
            orig,
            NULL
        };
        Error(m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
            "Fetch @ value not found @\n", args);
        return NULL;
    }
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->val.targets = Span_Make(m);
    return fch;
}
