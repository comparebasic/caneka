#include <external.h>
#include <caneka.h>

static boolean argHasFlag(Hashed *h, word flag){
    return h->value->type.of == TYPE_WRAPPED_PTR && (h->value->type.state & flag);
}

status Args_Add(Table *resolve, Str *key, Abstract *value, word flags){
    if(value == NULL){
        value = (Abstract *)Ptr_Wrapped(resolve->m, NULL, ZERO);
    }
    if(flags & ARG_CHOICE){
        value = (Abstract *)Ptr_Wrapped(resolve->m, as(value, TYPE_SPAN), TYPE_SPAN); 
    }
    value->type.state |= flags;
    Hashed *h = Table_SetHashed(resolve, (Abstract *)key, value);
    return SUCCESS;
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
                (Abstract *)h->key,
                NULL
            };

            if(argHasFlag(h, ARG_OPTIONAL)){
                Out("[$]", args);
            }else if(argHasFlag(h, ARG_MULTIPLE)){
                Out("[$, ...]", args);
            }else if(argHasFlag(h, ARG_CHOICE)){
                if(argHasFlag(h, ARG_DEFAULT)){
                    Span *p = ((Single *)h->value)->val.ptr;
                    Abstract *args[] = {
                        (Abstract *)h->key,
                        (Abstract *)p,
                        (Abstract *)Span_Get(p, 0),
                        NULL
                    };
                    Out("$(@=$)", args);
                }else{
                    Abstract *args[] = {
                        (Abstract *)h->key,
                        (Abstract *)((Single *)h->value)->val.ptr,
                        NULL
                    };
                    Out("$(@)", args);
                }
            }else if(argHasFlag(h, ARG_DEFAULT)){
                Abstract *args[] = {
                    (Abstract *)h->key,
                    (Abstract *)((Single *)h->value)->val.ptr,
                    NULL
                };
                Out("$(=$)", args);
            }else{
                Out("$", args);
            }

            if((it.type.state & LAST) == 0){
                Out(" ", NULL);
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
    if(argc < 1){
        return NOOP;
    }else{
        argv++;
        for(i32 i = 1; i < argc; i++, argv++){
            Str *s = Str_CstrRef(m, *argv);
            if(s->length > 1 && s->bytes[0] == '-' && s->bytes[1] == '-'){
                if(it.metrics.set != -1){
                    Table_SetValue(&it, (Abstract *)Ptr_Wrapped(m, NULL, 0));
                }
                it.metrics.set = -1;
                Str_Incr(s, 2);
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
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Unable to find resolve for arg @", args);
                }
            }else if(it.metrics.set != -1){
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
                Abstract *value = Table_Get(dest, h->key);
                Single *sg = NULL;
                if(h->value != NULL && h->value->type.of == TYPE_WRAPPED_PTR){
                    sg = (Single *)h->value;
                }

                if(argHasFlag(h, ARG_DEFAULT) && value == NULL && sg != NULL){
                    value = sg->val.ptr;
                    Table_Set(dest, h->key, (Abstract *)sg->val.ptr);
                }else if(!argHasFlag(h, ARG_OPTIONAL)){
                    if(value == NULL){
                        Abstract *args[] = {
                            (Abstract *)h->key,
                            NULL,
                        };
                        Error(m, FUNCNAME, FILENAME, LINENUMBER,
                            "Required argument not found: @", args);
                        exit(1);
                    }
                }

                if(argHasFlag(h, ARG_CHOICE) && value != NULL && sg != NULL){
                    Span *p = (Span *)as(sg->val.ptr, TYPE_SPAN);
                    if(Span_Has(p, value) == -1){
                        if(argHasFlag(h, ARG_DEFAULT)){
                             Table_Set(dest, h->key, Span_Get(p, 0));
                        }else{
                            Abstract *args[] = {
                                (Abstract *)h->key,
                                (Abstract *)p,
                                value,
                                NULL,
                            };
                            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                                "Required argument @ not in expected choices: @, found @",
                                    args);
                            exit(1);
                        }
                    }
                }

                if(argHasFlag(h, ARG_ABS_PATH) && value != NULL && value->type.of == TYPE_STR){
                     value = (Abstract *)IoUtil_GetAbsPath(m, (Str *)value);
                     Table_Set(dest, h->key, value);
                }
            }
        }
    }
    return r;
}
