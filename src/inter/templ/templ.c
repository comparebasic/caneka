#include <external.h>
#include <caneka.h>

static i32 Templ_FindStart(Templ *templ, word flags){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));

    if(flags == ZERO){
        flags = (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT);
    }

    Abstract *self = Iter_Get(&it);
    status latest = ((TemplJump *)self)->fch->type.state;

    i32 targetCount = 1;
    while((Iter_Prev(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_TEMPL_JUMP){
            TemplJump *prevJump = (TemplJump *)a;
            if(prevJump->fch->type.state & FETCHER_VAR){
                continue;
            }else if(prevJump->fch->type.state & FETCHER_END){
                targetCount++;
                latest = a->type.state;
            }else if((prevJump->fch->type.state & flags) == 0){
                if(prevJump->fch->type.state != latest && targetCount > 0){
                    --targetCount;
                }
                latest = prevJump->fch->type.state;
            }else if(prevJump->fch->type.state & flags){
                if(prevJump->fch->type.state != latest && targetCount > 0){
                    --targetCount;
                }
                latest = prevJump->fch->type.state;
                if(targetCount == 0){
                    DebugStack_Pop();
                    return it.idx;
                }
            }
        }
    }
    DebugStack_Pop();
    return -1;
}

static i32 Templ_FindNext(Templ *templ, status flags){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));

    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            if(a->type.state & FETCHER_VAR){
                continue;
            }else if(a->type.state & flags){
                return it.idx; 
            }
        }
    }
    DebugStack_Pop();
    return -1;
}


static i32 Templ_FindEnd(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));


    Abstract *self = Iter_Get(&it);
    status latest = ((TemplJump *)self)->fch->type.state;
    void *args[] = {self, NULL};

    i32 targetCount = 1;
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);

        if(a->type.of == TYPE_FETCHER){
            if(a->type.state & FETCHER_VAR){
                continue;
            }else if((a->type.state & FETCHER_END) == 0){
                void *args[] = {a, I32_Wrapped(templ->m, targetCount), NULL};
                if(a->type.state != latest){
                    targetCount++;
                }
                latest = a->type.state;
            }else if((a->type.state & FETCHER_END) && --targetCount == 0){
                DebugStack_Pop();
                return it.idx; 
            }
        }
    }
    DebugStack_Pop();
    return -1;
}

static status Templ_handleJump(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    status r = READY;
    void *args[5];
    TemplJump *jump = (TemplJump *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);

    Fetcher *fch = jump->fch;
    i32 skipIdx = -1;
    if(fch->type.state & FETCHER_END){
        skipIdx = templ->content.idx;
        TemplJump *dest = (TemplJump *)Span_Get(templ->content.p, jump->destIdx);
        if(dest->fch->type.state & FETCHER_WITH){
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }
        if(dest->fch->type.state & FETCHER_FOR){
            Iter_GetByIdx(&templ->content, jump->destIdx);
            jump = (TemplJump *)Iter_Get(&templ->content);
            fch = jump->fch;
        }
    }

    if(1 || templ->type.state & DEBUG){
        void *args[] = {
            jump,
            fch,
            NULL
        };
        Out("^b.  Jump: & \n  of &^0.\n", args);
    }

    if(fch->type.state & FETCHER_FOR){
        if((fch->type.state & PROCESSING) == 0){
            DebugStack_SetRef(fch, fch->type.of);
            void *value = as(Fetch(templ->m, fch, data, NULL), TYPE_ITER);
            Iter_Add(&templ->data, value);
            fch->type.state |= PROCESSING;
        }else{
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }
        Iter *it = (Iter *)Iter_Get(&templ->data);
        if(it->type.of != TYPE_ITER){
            void *args[] = { NULL, it, Iter_Get(&templ->data), NULL };
            Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error ^{STACK.name}, expected Iter have @ from Iter_Get(@) instead^0\n", 
            args);
            templ->type.state |= ERROR;
            return templ->type.state;
        }

        while((fch->api->next(it) & END) == 0){
            void *a = fch->api->get(it);
            void *ar[] = {a, it->p, NULL};
            Out("^p.api->get @ @^0\n", ar);
            if(a != NULL){
                Iter_Add(&templ->data, a);
                break;
            }
        }

        if(it->type.state & END){
            i32 idx = jump->skipIdx != NEGATIVE ? jump->skipIdx : skipIdx;
            if(idx != NEGATIVE){
                Iter_GetByIdx(&templ->content, idx);
                Iter_Remove(&templ->data);
                Iter_Prev(&templ->data);
            }
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_IF){
        if(templ->type.state & DEBUG){
            void *args[] = {
                fch,
                data,
                NULL
            };
            Out("^c.  If: & of @^0.\n", args);
        }
        fch->type.state |= PROCESSING;
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){
            if(templ->type.state & DEBUG){
                void *args[] = {
                    fch,
                    data,
                    NULL
                };
                Out("^c.  If is null: & of @^0.\n", args);
            }
            Iter_GetByIdx(&templ->content, jump->skipIdx);
            r |= PROCESSING;
        }else{
            Iter_Add(&templ->data, value);
            if(templ->type.state & DEBUG){
                void *args[] = {
                    fch,
                    Iter_Get(&templ->data),
                    NULL
                };
                Out("^c.  If nesting/found: & of @^0.\n", args);
            }
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_WITH){
        if(templ->type.state & DEBUG){
            void *args[] = {
                fch,
                data,
                NULL
            };
            Out("^c.  With: & of @^0.\n", args);
        }
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        Iter_Add(&templ->data, value);
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_CONDITION){
        if(templ->type.state & DEBUG){
            void *args[] = {
                fch,
                NULL
            };
            Out("^c.  Command: &^0.\n", args);
        }
        if(fch->val.targets->nvalues == 0){
            Iter_GetByIdx(&templ->content, jump->destIdx);
            r |= PROCESSING;
        }else{
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            if(tg->objType.of == FORMAT_TEMPL_LEVEL){
            }else if(tg->objType.of == FORMAT_TEMPL_CURRENT){
            }else if(tg->objType.of == FORMAT_TEMPL_ACTIVE){
            }else{
                Iter_GetByIdx(&templ->content, jump->skipIdx);
                r |= PROCESSING;
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}

static status Templ_PrepareCycle(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    void *args[5];
    MemCh *m = templ->m;
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= PROCESSING;
        DebugStack_Pop();
        return templ->type.state;
    }

    Abstract *item = Iter_Get(&templ->content);
    if(templ->type.state & DEBUG){
        args[0] = NULL;
        args[1] = templ;
        args[2] = item;
        args[3] = NULL;
        Out("^c.^{STACK.name}: Templ:&\n    item:&^0\n", args);
    }

    word unregJumpFl = (FETCHER_FOR|FETCHER_CONDITION|FETCHER_END|FETCHER_WITH|FETCHER_IF);
    if(item->type.of == TYPE_FETCHER && (((Fetcher *)item)->type.state & unregJumpFl)){
        Fetcher *fch = (Fetcher*)item;

        i32 idx = templ->content.idx;
        TemplJump *jump = TemplJump_Make(m, idx, fch);
        Span_Set(templ->content.p, idx, jump);
        Iter_GetByIdx(&templ->content, idx);

        if(fch->type.state & FETCHER_FOR){
            jump->level = 0;
            jump->skipIdx = Templ_FindEnd(templ);
            jump->destIdx = Templ_FindNext(templ, FETCHER_CONDITION);
        }else if(fch->type.state & (FETCHER_WITH|FETCHER_IF)){
            jump->skipIdx = Templ_FindEnd(templ); 
        }else if((fch->type.state & (FETCHER_CONDITION|FETCHER_TEMPL)) == 
                (FETCHER_CONDITION|FETCHER_TEMPL)){
            jump->skipIdx = Templ_FindEnd(templ);
            jump->destIdx = Templ_FindStart(templ, ZERO);
        }else if(fch->type.state & (FETCHER_CONDITION)){
            FetchTarget *tg = Span_Get(jump->fch->val.targets, 0); 
            if(tg != NULL && tg->objType.of == FORMAT_TEMPL_INDENT){
                jump->destIdx = Templ_FindStart(templ, ZERO);
            }else{
                jump->destIdx = Templ_FindEnd(templ);
            }
            jump->skipIdx = Templ_FindNext(templ, (FETCHER_CONDITION|FETCHER_END));
        }else if(fch->type.state & FETCHER_END){
            i32 destIdx = Templ_FindStart(templ, ZERO);
            if(destIdx > -1){
                TemplJump *dest = Span_Get(templ->content.p, destIdx);
                if(dest != NULL && 
                        (dest->fch->type.state & FETCHER_WITH|FETCHER_FOR)){
                    jump->destIdx = destIdx;
                }else{
                    jump->fch->type.state |= NOOP;
                }
            }
        }

        if((jump->fch->type.state & (NOOP|FETCHER_FOR|FETCHER_WITH)) == 0 &&
                jump->destIdx == NEGATIVE && jump->skipIdx == NEGATIVE
            ){
            args[0] = jump;
            args[1] = I32_Wrapped(m, templ->content.idx);
            args[2] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                "Unable to find source or ending for Jump in Templ content: @ \\@$", 
                args);

            templ->type.state |= ERROR;
            DebugStack_Pop();
            return templ->type.state;
        }

        if(templ->type.state & DEBUG){
            void *args[] = {
                jump,
                NULL,
            };
            Out("^c.  Making a Jump &^0.\n", args);
        }
    }

    if(templ->type.state & DEBUG){
        void *args[] = {
            Iter_Get(&templ->content),
            NULL
        };
        Out("^y.Templ_Prepare end\n  ^E.content:^e.&^0.\n", args);
    }

    DebugStack_Pop();
    return templ->type.state;
}

i64 Templ_ToSCycle(Templ *templ, Buff *bf, i64 total, void *source){
    DebugStack_Push(templ, templ->type.of);
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        DebugStack_Pop();
        return total;
    }

    Abstract *item = Iter_Get(&templ->content);
    DebugStack_SetRef(item, item->type.of);

    if(item->type.of == TYPE_TEMPL_JUMP){
        if(Templ_handleJump(templ) & (PROCESSING|SUCCESS)){
            item = Iter_Get(&templ->content);
        }
    }

    Abstract *data = Iter_Get(&templ->data);


    if(1 || (templ->type.state & DEBUG)){
        void *args[] = {
            NULL,
            item,
            Type_ToStr(templ->m, data->type.of),
            data,
            NULL
        };
        Out("^0.^{STACK.name}^y.\n  (item:@)\n  (data-typeOf/@: @)^0.\n", args);
    }


    if(item->type.of == TYPE_STRVEC){
        templ->m->level--;
        total += ToS(bf, item, 0, ZERO); 
        templ->m->level++;
    }else if(item->type.of == TYPE_FETCHER && item->type.state & FETCHER_VAR){
        Fetcher *fch = (Fetcher *)item;
        if(templ->type.state & DEBUG){
            void *args[] = {
                fch,
                NULL
            };
            Out("^c.  Fetcher: &^0.\n", args);
        }

        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){
            void *args[] = {
                fch,
                data,
                NULL,
            };
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error finding value using @ in data @\n",args);
            DebugStack_Pop();
            return total;
        }
        if(templ->type.state & DEBUG){
            void *args[] = {
                value,
                NULL
            };
            Out("^c.  VarValue: &^0.\n", args);
        }
        templ->m->level--;
        total += ToS(bf, value, 0, ZERO); 
        templ->m->level++;
    }

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    if(templ->type.state & DEBUG){
        void *args[] = {
            bf->v,
            NULL
        };
        Out("^yE.>> Out:^e. @^0.\n", args);
    }

    DebugStack_Pop();
    return total;
}

status Templ_Prepare(Templ *templ){
    DebugStack_Push(templ, templ->type.of);

    printf("prepare\n");
    fflush(stdout);

    if((templ->type.state & PROCESSING) == 0){
        i16 g = 0;
        templ->type.state &= ~(ERROR|SUCCESS);
        while((Templ_PrepareCycle(templ) & (ERROR|PROCESSING)) == 0){
            Guard_Incr(templ->m, &g, 64, FUNCNAME, FILENAME, LINENUMBER);
        }
        if((templ->type.state & PROCESSING) == 0){
            void *args[] = {
                templ,
                NULL
            };
            Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing Templ, &", args);
            DebugStack_Pop();
            return ERROR;
        }
        DebugStack_Pop();
        return (templ->type.state|SUCCESS);
    }
    DebugStack_Pop();
    return NOOP;
}

status Templ_SetData(Templ *templ, void *data){
    Span *p = Span_Make(templ->m);
    Span_Add(p, data);
    Iter_Init(&templ->data, p);
    Iter_Next(&templ->data);
    return ZERO;
}

i64 Templ_ToS(Templ *templ, Buff *bf, void *data, void *source){
    templ->type.state &= ~SUCCESS;
    templ->m->level++; 
    DebugStack_Push(templ, templ->type.of);
    i64 total = 0;
    i16 g = 0;

    if(Templ_Prepare(templ) & ERROR){
        DebugStack_Pop();
        return 0;
    }

    printf("ToS\n");
    fflush(stdout);

    if(data != NULL){
        Templ_SetData(templ, data);
    }

    while((total = Templ_ToSCycle(templ, bf, total, source)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(templ->m, &g, 64, FUNCNAME, FILENAME, LINENUMBER);
    }
    DebugStack_Pop();
    templ->m->level--; 
    Templ_Reset(templ);
    templ->type.state |= SUCCESS;
    return total;

}

status Templ_Reset(Templ *templ){
    templ->level = 0;
    templ->indent = 0;
    templ->type.state &= DEBUG;
    templ->content.type.state |= END;
    MemCh_FreeTemp(templ->m);
    while((Iter_Next(&templ->content) & END) == 0){
        Fetcher *fch = NULL;
        Abstract *a = Iter_Get(&templ->content);
        if(a->type.of == TYPE_TEMPL_JUMP){
            fch = ((TemplJump *)a)->fch;
        }else if (a->type.of == TYPE_FETCHER){
            fch = (Fetcher *)fch;
        }
        if(fch != NULL){
            fch->type.state &= ~PROCESSING;
        }
    }
    Iter_Init(&templ->content, templ->content.p);
    return SUCCESS;
}

Templ *Templ_Make(MemCh *m, Span *content){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    templ->m = m;
    Iter_Init(&templ->content, content);
    return templ;
}
