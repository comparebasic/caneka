#include <external.h>
#include <caneka.h>

static boolean argHasFlag(Hashed *h, word flag){
    return h->value->type.of == TYPE_WRAPPED_PTR && (h->value->type.state & flag);
}

status CharPtr_ToHelp(MemCh *m, Str *name, Table *resolve, int argc, char **argv){
    Abstract *args[] = {
        (Abstract *)name,
        NULL
    };
    Out("$ ", args);
    Iter it;
    Iter_Init(&it, resolve);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Abstract *args[] = {
                (Abstract *)h->item,
                NULL
            };

            if(argHasFlag(h, ARG_OPTIONAL)){
                Out("[$] ", args);
            }else if(argHasFlag(h, ARG_MULTIPLE)){
                Out("[$, ...] ", args);
            }else{
                Out("$ ", args);
            }
        }
    }
    Out("\n", NULL);
    exit(1);
}

status CharPtr_ToTbl(MemCh *m, Table *resolve, int argc, char **argv, Table *dest){
    status r = READY;
    Str *key = NULL;
    Abstract *rslv = NULL;
    Iter it;
    Iter_Init(&it, dest);
    if(argc < 2){
        return NOOP;
    }else{
        argv++;
        for(i32 i = 1; i < argc; i++, argv++){
            Str *s = Str_CstrRef(m, *argv);
            if(s->bytes[0] == '-'){
                if(it.metrics.set != -1){
                    Table_SetValue(&it, (Abstract *)Ptr_Wrapped(m, NULL, 0));
                }
                it.metrics.set = -1;
                Str_Incr(s, 1);
                Abstract *arg = Table_Get(resolve, (Abstract *)s);
                if(arg != NULL){
                    key = s;
                    rslv = arg;
                    Table_SetKey(&it, (Abstract *)key);
                }else{
                    Abstract *args[] = {
                        (Abstract *)s,
                        NULL
                    };
                    Error(ErrStream->m, (Abstract *)resolve, 
                        FUNCNAME, FILENAME, LINENUMBER, "Unable to find resolve for arg @",
                        args);
                }
            }else if(it.metrics.set != -1){
                 if(rslv != NULL/* && rslv->type.of == TYPE_WRAPPED_PTR*/){
                     if(rslv->type.state & ARG_ABS_PATH){
                        s = File_GetAbsPath(m, s);
                     }
                 }
                 Table_SetValue(&it, (Abstract *)s);
                 key = NULL;
                 rslv = NULL;
                 r |= SUCCESS;
            }else{
                if(it.metrics.set == -1){
                    Table_SetKey(&it, (Abstract *)I32_Wrapped(m, i));
                }
                Table_SetValue(&it, (Abstract *)s);
            }
        }
        Iter it;
        Iter_Init(&it, resolve);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                if(!argHasFlag(h, ARG_OPTIONAL)){
                    if(Table_Get(dest, h->item) == NULL){
                        Abstract *args[] = {
                            (Abstract *)h->item,
                            NULL,
                        };
                        Error(m, (Abstract *)h->item, FUNCNAME, FILENAME, LINENUMBER,
                            "Required argument not found: @", args);
                        exit(1);
                    }
                }
            }
        }
    }
    return r;
}
