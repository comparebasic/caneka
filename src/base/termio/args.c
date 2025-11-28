#include <external.h>
#include <caneka.h>
#include "../module.h"

static boolean argHasFlag(Hashed *h, word flag){
    return ((Abstract *)h->value)->type.of == TYPE_WRAPPED_PTR &&
        (((Abstract *)h->value)->type.state & flag);
}

status Args_ErrorFunc(MemCh *m, void *_cliArgs, void *_msg){
    CliArgs *cli = _cliArgs;
    ErrorMsg *msg = _msg;
    Fmt(ErrStream, (char *)msg->fmt->bytes, msg->args);
    Buff_Add(ErrStream, S(m, "\nhelp:\n  "));
    CharPtr_ToHelp(m, cli->name, cli->resolve);
    return ERROR;
}

status Args_Add(Table *resolve, Str *key, void *_value, word flags){
    Abstract *value = (Abstract *)_value;
    if(value == NULL){
        value = (Abstract *)Ptr_Wrapped(resolve->m, NULL, ZERO);
    }
    if(flags & ARG_CHOICE){
        value = (Abstract *)Ptr_Wrapped(resolve->m, as(value, TYPE_SPAN), TYPE_SPAN); 
    }
    value->type.state |= flags;
    Hashed *h = Table_SetHashed(resolve, key, value);
    return SUCCESS;
}

status CharPtr_ToHelp(MemCh *m, Str *name, Table *resolve){
    void *args[] = {
        name,
        NULL
    };
    Out("$ ", args);
    Iter it;
    Iter_Init(&it, resolve);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            void *args[] = {
                h->key,
                NULL
            };

            if(argHasFlag(h, ARG_OPTIONAL)){
                Out("[$]", args);
            }else if(argHasFlag(h, ARG_MULTIPLE)){
                Out("[$, ...]", args);
            }else if(argHasFlag(h, ARG_CHOICE)){
                if(argHasFlag(h, ARG_DEFAULT)){
                    Span *p = ((Single *)h->value)->val.ptr;
                    void *args[] = {
                        h->key,
                        p,
                        Span_Get(p, 0),
                        NULL
                    };
                    Out("$(@=$)", args);
                }else{
                    void *args[] = {
                        h->key,
                        ((Single *)h->value)->val.ptr,
                        NULL
                    };
                    Out("$(@)", args);
                }
            }else if(argHasFlag(h, ARG_DEFAULT)){
                void *args[] = {
                    h->key,
                    ((Single *)h->value)->val.ptr,
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
    void *rslv = NULL;
    Iter it;
    Iter_Init(&it, dest);
    if(argc < 1){
        return NOOP;
    }else{
        argv++;
        for(i32 i = 1; i < argc; i++, argv++){
            Str *s = Str_CstrRef(m, *argv);
            if(s->length > 1 && s->bytes[0] == '-' && s->bytes[1] == '-'){
                if(it.metrics.selected != -1){
                    Table_SetValue(&it, Ptr_Wrapped(m, NULL, 0));
                }
                it.metrics.selected = -1;
                Str_Incr(s, 2);
                void *arg = Table_Get(resolve, s);
                if(arg != NULL){
                    key = s;
                    rslv = arg;
                    Table_SetKey(&it, key);
                }else{
                    void *args[] = {
                        s,
                        NULL
                    };
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Unable to find resolve for arg @", args);
                }
            }else if(it.metrics.selected != -1){
                 Table_SetValue(&it, s);
                 key = NULL;
                 rslv = NULL;
                 r |= SUCCESS;
            }else{
                if(it.metrics.selected == -1){
                    Table_SetKey(&it, I32_Wrapped(m, i));
                }
                Table_SetValue(&it, s);
            }
        }
        Iter it;
        Iter_Init(&it, resolve);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                void *value = Table_Get(dest, h->key);
                Single *sg = NULL;
                if(h->value != NULL && 
                        ((Abstract *)h->value)->type.of == TYPE_WRAPPED_PTR){
                    sg = (Single *)h->value;
                }

                if(argHasFlag(h, ARG_DEFAULT) && value == NULL && sg != NULL){
                    value = sg->val.ptr;
                    Table_Set(dest, h->key, sg->val.ptr);
                }else if(!argHasFlag(h, ARG_OPTIONAL)){
                    if(value == NULL){
                        void *args[] = {
                            h->key,
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
                            void *args[] = {
                                h->key,
                                p,
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

                if(argHasFlag(h, ARG_ABS_PATH) && value != NULL &&
                        ((Abstract *)value)->type.of == TYPE_STR){
                    value = IoUtil_GetAbsPath(m, (Str *)value);
                    Table_Set(dest, h->key, value);
                }
            }
        }
    }
    return r;
}

CliArgs *CliArgs_Make(MemCh *m, i32 argc, char *argv[]){
    CliArgs *args = (CliArgs *)MemCh_AllocOf(m, sizeof(CliArgs), TYPE_CLI_ARGS);
    args->type.of = TYPE_CLI_ARGS;
    args->resolve = Table_Make(m);
    args->args = Table_Make(m);
    args->argc = argc;
    if(argv != NULL){
        args->name = IoUtil_FnameStr(m, IoPath(m, argv[0]));
    }

    args->argv = argv;
    return args;
}

status Args_Init(MemCh *m){
    status r = READY;
    Lookup_Add(m, ErrorHandlers, TYPE_CLI_ARGS, (void *)Args_ErrorFunc);
    return r;
}
