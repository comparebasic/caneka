#include <external.h>
#include <caneka.h>

i32 TemplCrit_Decr(Templ *templ, status flags){
    TemplCrit *crit = Iter_Get(&templ->ret);
    while((crit == NULL || crit->incr <= 0) && 
            (Iter_Next(&templ->ret) & END) == 0){
        crit = Iter_Get(&templ->ret);
    }
    if(crit != NULL &&
            (IterUpper_FlagCombine(crit->type.state, flags) & SUCCESS)){
        crit->incr--;
        return crit->idx;
    } 
    return -1;
}

status TemplCrit_Reset(Templ *templ){
    Span_Wipe(templ->ret.p);
    Iter_Init(&templ->ret, templ->ret.p);
    return ZERO;
}

status TemplCrit_Add(Templ *templ, i32 idx, status flags){
    TemplCrit *crit = Span_Get(templ->ret.p, templ->ret.p->max_idx);
    if(crit != NULL && crit->type.state == flags && crit->idx == idx){
        crit->incr++;
    }else{
        crit = TemplCrit_Make(templ->m);
        crit->idx = idx;
        crit->type.state = flags;
        crit->incr = 1;
        Span_Add(templ->ret.p, crit);
        Iter_GetByIdx(&templ->ret, templ->ret.idx);
    }
    return ZERO;
}

TemplCrit *TemplCrit_Make(MemCh *m){
    TemplCrit *crit = MemCh_AllocOf(m, sizeof(TemplCrit), TYPE_TEMPL_JUMP_CRIT);
    crit->type.of = TYPE_TEMPL_JUMP_CRIT;
    crit->idx = -1;
    return crit;
}
