#include <external.h>
#include "base_module.h"

static boolean argHasFlag(Hashed *h, word flag){
    Abstract *value = h->value;
    return h->value != NULL && (value->type.state & flag);
}

static status CharPtr_ToTbl(MemCh *m, Table *resolve, i32 argc, char **argv, Table *dest){
    status r = READY;
    Str *key = NULL;
    Iter it;
    Iter_Init(&it, dest);
    if(argc < 1){
        return NOOP;
    }else{
        /* build args pass */
        Single *current = NULL;
        for(i32 i = 1; i < argc; i++){
            Str *s = S(m, argv[i]);
            if(s->length > 1 && s->bytes[0] == '-' && s->bytes[1] == '-'){
                if(it.metrics.selected != -1){
                    Table_SetValue(&it, Ptr_Wrapped(m, NULL, 0));
                    current = NULL;
                }
                it.metrics.selected = -1;
                Str_Incr(s, 2);
                current = Table_Get(resolve, s);
                if(current != NULL){
                    key = s;
                    Table_SetKey(&it, key);
                    if(current->type.state & ARG_MULTIPLE){
                        i32 selected = it.metrics.selected;
                        Span *p = Span_Make(m);
                        Table_SetValue(&it, p);
                        it.metrics.selected = selected;
                    }
                }else{
                    void *args[] = {
                        s,
                        NULL
                    };
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Unable to find resolve for arg @", args);
                }
            }else if(it.metrics.selected != -1){
                if(current != NULL && current->type.state & ARG_MULTIPLE){
                    Hashed *h = Iter_GetSelected(&it);
                    Span *p = h->value;
                    Span_Add(p, s);
                    it.metrics.selected = -1;
                }else{
                    Table_SetValue(&it, s);
                }

                key = NULL;
                current = NULL;
                r |= SUCCESS;
            }else{
                if(it.metrics.selected == -1){
                    Table_SetKey(&it, I32_Wrapped(m, i));
                }
                Table_SetValue(&it, s);
                current = NULL;
            }
        }
        /* verify completeness */
        Iter it;
        Iter_Init(&it, Table_Ordered(m, resolve));
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
                        return ERROR;
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
                            return ERROR;
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

status Args_ErrorFunc(MemCh *m, void *_cliArgs, void *_msg){
    CliArgs *cli = _cliArgs;
    ErrorMsg *msg = _msg;
    Buff_Add(ErrStream, S(m, "help:\n\n"));
    CharPtr_ToHelp(cli);

    void *args[] = {
        Ptr_Wrapped(m, cli->argv, TYPE_CSTR_ARRAY),
        cli->args,
        NULL
    };

    Out("^p.Command line arguments recieved: @, gathered: @^0\n", args);

    return ERROR;
}

status Args_Add(CliArgs *cli, Str *key, void *_value, word flags, StrVec *explain){
    Abstract *value = (Abstract *)_value;
    if(value == NULL){
        value = (Abstract *)Ptr_Wrapped(cli->m, NULL, ZERO);
    }else{
        if(flags & ARG_CHOICE){
            as(value, TYPE_SPAN); 
        }
        value = (Abstract *)Ptr_Wrapped(cli->m, value, value->type.of); 
    }
    value->type.state |= flags;
    Hashed *h = Table_SetHashed(cli->resolve, key, value);
    if(explain != NULL){
        Span_Set(cli->explain, h->orderIdx, explain);
    }
    return SUCCESS;
}

status CharPtr_ToHelp(CliArgs *cli){
    MemCh *m = cli->m;
    void *args[] = {
        cli->name,
        NULL
    };

    Str *noColorKey = K(m, "no-color");
    if(Table_GetHashed(cli->args, noColorKey) != NULL){
        Ansi_SetColor(FALSE);
    }

    Out("^c.$^0.\n\n", args);
    Iter it;
    Iter_Init(&it, Table_Ordered(m, cli->resolve));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            void *args[] = {
                h->key,
                Span_Get(cli->explain, h->orderIdx),
                NULL
            };

            if(argHasFlag(h, ARG_OPTIONAL)){
                if(argHasFlag(h, ARG_MULTIPLE)){
                    Out(" [ ^y.--$^0. ... ]\n $\n", args);
                }else{
                    Out(" [ ^y.--$^0. ]\n $\n", args);
                }
            }else if(argHasFlag(h, ARG_CHOICE)){
                if(argHasFlag(h, ARG_DEFAULT)){
                    Span *p = ((Single *)h->value)->val.ptr;
                    void *args[] = {
                        h->key,
                        p,
                        Span_Get(p, 0),
                        Span_Get(cli->explain, h->orderIdx),
                        NULL
                    };
                    Out(" [^y. --$^0.(@=@) ]\n $\n", args);
                }else{
                    void *args[] = {
                        h->key,
                        ((Single *)h->value)->val.ptr,
                        Span_Get(cli->explain, h->orderIdx),
                        NULL
                    };
                    Out(" ^y.--$^0.(@)\n $\n", args);
                }
            }else if(argHasFlag(h, ARG_DEFAULT)){
                void *args[] = {
                    h->key,
                    ((Single *)h->value)->val.ptr,
                    Span_Get(cli->explain, h->orderIdx),
                    NULL
                };
                Out(" [ ^y.--$^0.(=@) ]\n $\n", args);
            }else{
                if(argHasFlag(h, ARG_MULTIPLE)){
                    Out(" ^y.--$^0. ...\n $\n", args);
                }else{
                    Out(" ^y.--$^0.\n $\n", args);
                }
            }

            if((it.type.state & LAST) == 0){
                Out("\n", NULL);
            }
        }
    }
    Out("\n", NULL);
    return ZERO;
}

void CliArgs_Free(CliArgs *cli){
    MemCh_Free(cli->m);
}

status CliArgs_Parse(CliArgs *cli){
    status r = CharPtr_ToTbl(cli->m, cli->resolve, cli->argc, cli->argv, cli->args);
    Str *helpKey = K(cli->m, "help");
    if(Table_GetHashed(cli->args, helpKey) != NULL){
        CharPtr_ToHelp(cli);
        return NOOP;
    }
    return r;
}

void *CliArgs_Get(CliArgs *cli, void *key){
    Hashed *h = Table_GetHashed(cli->args, key);
    if(h != NULL){
        if(h->value == NULL){
            return I64_Wrapped(cli->m, 1);
        }else{
            return h->value;
        }
    }
    return NULL;
}

StrVec *CliArgs_GetAbsPath(CliArgs *cli, void *key){
    Str *path = (Str *)as(CliArgs_Get(cli, key), TYPE_STR);
    return IoUtil_AbsVec(cli->m, StrVec_From(cli->m, path));
}

CliArgs *CliArgs_Make(i32 argc, char *argv[]){
    MemCh *m = MemCh_Make();
    CliArgs *cli = (CliArgs *)MemCh_AllocOf(m, sizeof(CliArgs), TYPE_CLI_ARGS);
    cli->type.of = TYPE_CLI_ARGS;
    cli->m = m;
    cli->resolve = Table_Make(m);
    cli->args = Table_Make(m);
    cli->explain = Span_Make(m);
    cli->argc = argc;
    if(argv != NULL){
        cli->name = IoUtil_FnameStr(m, IoPath(m, argv[0]));
    }

    cli->argv = argv;
    m->owner = cli;
    return cli;
}

status Args_Init(MemCh *m){
    status r = READY;
    Lookup_Add(m, ErrorHandlers, TYPE_CLI_ARGS, (void *)Args_ErrorFunc);
    return r;
}
