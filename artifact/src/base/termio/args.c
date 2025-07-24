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
                Out("[$]", args);
            }else if(argHasFlag(h, ARG_MULTIPLE)){
                Out("[$, ...]", args);
            }else if(argHasFlag(h, ARG_DEFAULT)){
                Abstract *args[] = {
                    (Abstract *)h->item,
                    (Abstract *)((Single *)h->value)->val.ptr,
                    NULL
                };
                Out("$(=$)", args);
            }else if(argHasFlag(h, ARG_CHOICE)){
                Abstract *args[] = {
                    (Abstract *)h->item,
                    (Abstract *)((Single *)h->value)->val.ptr,
                    NULL
                };
                Out("$(@)", args);
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
    if(argc < 2){
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
                    Error(ErrStream->m, (Abstract *)resolve, 
                        FUNCNAME, FILENAME, LINENUMBER, "Unable to find resolve for arg @",
                        args);
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
                Abstract *value = Table_Get(dest, h->item);
                Single *sg = NULL;
                if(h->value != NULL && h->value->type.of == TYPE_WRAPPED_PTR){
                    sg = (Single *)h->value;
                }


                if(argHasFlag(h, ARG_DEFAULT) && value == NULL && sg != NULL){
                    value = sg->val.ptr;
                    Table_Set(dest, h->item, (Abstract *)sg->val.ptr);
                }else if(!argHasFlag(h, ARG_OPTIONAL)){
                    if(value == NULL){
                        Abstract *args[] = {
                            (Abstract *)h->item,
                            NULL,
                        };
                        Error(m, (Abstract *)resolve, FUNCNAME, FILENAME, LINENUMBER,
                            "Required argument not found: @", args);
                        exit(1);
                    }
                }

                if(argHasFlag(h, ARG_CHOICE) && value != NULL && sg != NULL){
                    Span *p = (Span *)as(sg->val.ptr, TYPE_SPAN);
                    if(Span_Has(p, value) == -1){
                        Abstract *args[] = {
                            (Abstract *)h->item,
                            (Abstract *)p,
                            value,
                            NULL,
                        };
                        Error(m, (Abstract *)resolve, FUNCNAME, FILENAME, LINENUMBER,
                            "Required argument @ not in expected choices: @, found @", args);
                        exit(1);

                    }
                }

                if(argHasFlag(h, ARG_ABS_PATH) && value != NULL && value->type.of == TYPE_STR){
                     value = (Abstract *)File_GetAbsPath(m, (Str *)value);
                     Table_Set(dest, h->item, value);
                }
            }
        }
    }
    return r;
}
