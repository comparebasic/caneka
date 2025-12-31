#include <external.h>
#include <caneka.h>

status NestSel_Init(Iter *it, Inst *inst, Span *coords){
    status r = READY;
    if(it->p == NULL){
        Iter_Init(it, Span_Make(m));
    }
    
    Table *children = Span_Get(inst, INST_PROPIDX_CHILDREN);

    Iter _it;
    Iter_Init(&_it, coords);
    while((Iter_Next(&_it) & END) == 0){
        Single *sg = Iter_Get(&_it);
        Table *next = Span_Get(children, sg->val.i); 
        if(next == NULL){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Nested item expected and not found", NULL);
            return ERROR;
        }
        Iter *cur = Iter_GetByIdx(it, _it.idx);
        if(cur == NULL){
            cur = Iter_Make(m, children);
            Iter_SetByIdx(it, sg->val.i, cur);
        }
        cur->metrics.selected = sg->val.i;
        children = next;
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status NestSel_Next(Iter *it){
    return ZERO;
}
