#include <external.h>
#include <caneka.h>

i64 Templ_ToSCycle(Templ *templ, Buff *bf, i64 total, void *source){
    DebugStack_Push(NULL, ZERO);
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        DebugStack_Pop();
        return total;
    }

    Abstract *item = Iter_Get(&templ->content);
    DebugStack_SetRef(item, item->type.of);

    Abstract *prev = NULL;
    while(prev != item && item->type.of == TYPE_FETCHER){
        if(Templ_HandleJump(templ) & PROCESSING){
            prev = item;
            item = Iter_Get(&templ->content);
        }else{
            break;
        }
    }

    Abstract *data = Iter_Get(&templ->data);
    if(item->type.of == TYPE_STRVEC){
        templ->m->level--;
        total += ToS(bf, item, 0, ZERO); 

        void *ar[] = {item, NULL};
        Out("^p.OutVec: @^0\n", ar);

        templ->m->level++;
    }else if(item->type.of == TYPE_FETCHER && item->type.state & FETCHER_VAR){
        Fetcher *fch = (Fetcher *)item;
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
        templ->m->level--;

        void *ar[] = {value, NULL};
        Out("^p.OutVar: @^0\n", ar);

        total += ToS(bf, value, 0, ZERO); 
        templ->m->level++;
    }

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    DebugStack_Pop();
    return total;
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

    if(data != NULL){
        Templ_SetData(templ, data);
    }

    while((total = Templ_ToSCycle(templ, bf, total, source)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(templ->m, &g, 128, FUNCNAME, FILENAME, LINENUMBER);
    }
    DebugStack_Pop();
    templ->m->level--; 
    Templ_Reset(templ);
    templ->type.state |= SUCCESS;
    return total;

}

status Templ_Reset(Templ *templ){
    templ->type.state &= DEBUG;
    templ->content.type.state |= END;
    MemCh_FreeTemp(templ->m);
    while((Iter_Next(&templ->content) & END) == 0){
        Fetcher *fch = NULL;
        Abstract *a = Iter_Get(&templ->content);
        if(a->type.of == TYPE_FETCHER){
            fch = (Fetcher *)fch;
        }
        if(fch != NULL){
            fch->type.state &= ~PROCESSING;
        }
    }
    Iter_Init(&templ->content, templ->content.p);
    return SUCCESS;
}

status Templ_SetData(Templ *templ, void *data){
    Span *p = Span_Make(templ->m);
    Span_Add(p, data);
    ItinIt_Init(&templ->data, p);
    Iter_Next(&templ->data);
    return ZERO;
}

Templ *Templ_Make(MemCh *m, Span *content){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    templ->m = m;
    ItinIt_Init(&templ->content, content);
    templ->jumps = Lookup_Make(m, 0);
    return templ;
}
