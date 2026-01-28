#include <external.h>
#include <caneka.h>

static status Jump_NextSetFl =
    UFLAG_ITER_FOCUS|UFLAG_ITER_SIBLING|UFLAG_ITER_LEAF;

Lookup *TemplFuncLookup = NULL;

void Templ_IterNext(Templ *templ, TemplFunc *tfunc){
    MemCh *m = templ->m;
    Fetcher *fch = (Fetcher *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);
    Iter *it = NULL;
    tfunc->dflag.positive = ZERO;
    tfunc->dflag.negative = ZERO;
    if((fch->type.state & PROCESSING) == 0){
        Iter *value = as(Fetch(m, fch, data, NULL), TYPE_ITER);
        Itin_IterAdd(&templ->data, value);
        fch->type.state |= PROCESSING;
        it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
    }else{
        it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
        if((it->type.state & END) == 0){
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }
    }

    if(it == NULL || it->type.of != TYPE_ITER){
        void *args[] = { NULL, it, Iter_Get(&templ->data), NULL };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error ^{STACK.name}, expected Iter have @ from"
            " Iter_Get(@) instead^0\n", 
            args);
        templ->type.state |= ERROR;
        DebugStack_Pop();
        return;
    }

    i32 indentIdx = it->idx;
    if((it->type.state & END) || (fch->api->next(it) & END)){
        tfunc->dflag.positive |= UFLAG_ITER_FINISH;
        tfunc->dflag.negative |= UFLAG_ITER_ACTION;
        templ->level = 0;
    }else{
        Abstract *a = fch->api->get(it);
        Itin_IterAdd(&templ->data, a);
        tfunc->dflag.positive |= UFLAG_ITER_ACTION;

        if(it->itin != NULL){
            templ->level = it->idx;
            tfunc->dflag.negative = Jump_NextSetFl;
            tfunc->dflag.positive = (it->itin->objType.state & Jump_NextSetFl);

            if(templ->type.state & DEBUG){
                void *ar[] = {
                    Type_StateVec(m, TYPE_ITER_UPPER, it->itin->objType.state),
                    a,
                    NULL
                };
                Out("^b.Item itin @ @^0\n", ar);
            }
        }else{
            if(templ->type.state & DEBUG){
                void *ar[] = {
                    a,
                    Type_StateVec(m, TYPE_ITER, it->type.state),
                    NULL
                };
                Out("^b.Item @ @^0\n", ar);
            }

            tfunc->dflag.positive = UFLAG_ITER_LEAF;
            tfunc->dflag.negative = Jump_NextSetFl;
            if(it->idx > 0){
                tfunc->dflag.positive |= UFLAG_ITER_SIBLING;
            }
            if(it->idx == it->metrics.selected){
                tfunc->dflag.positive |= UFLAG_ITER_FOCUS;
            }
        }

        if(it->idx < indentIdx){
            tfunc->dflag.positive |= UFLAG_ITER_FINISH;
        }
    }
}

status Templ_Indent(Templ *templ){
    return NOOP;
}

Func *Templ_GetFunc(cls typeOf){
    return Lookup_Get(TemplFuncLookup, typeOf);
}

status Templ_FuncInit(MemCh *m){
    status r = READY;
    if(TemplFuncLookup == NULL){
        TemplFuncLookup = Lookup_Make(m, ZERO);
    }
    r |= Lookup_Add(m, TemplFuncLookup, TYPE_ITER, (void *)Templ_IterNext);
    r |= Lookup_Add(m, TemplFuncLookup, FORMAT_TEMPL_INDENT, (void *)Templ_Indent);
    return r;
}
