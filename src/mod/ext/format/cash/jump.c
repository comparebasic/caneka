#include <external.h>
#include <caneka.h>

void Jump_FindSource(Jump *jmp, Iter *it){
    Iter temp;
    memcpy(&temp, it, sizeof(Iter));
    i32 skip = 1;
    i32 jumpIdx = temp.idx;
    while((Iter_Prev(&temp) & END) == 0){
        Abstract *a = Iter_Get(&temp);
        if(a != jmp && a->type.of == TYPE_CASH_JUMP && ((Jump *)a)->objType.of == CASH_END){
            skip++;
        }else if(a->type.of == TYPE_FETCHER){
            Fetcher *fch = (Fetcher *)a;
            Abstract *tail = Span_Get(fch->val.targets, fch->val.targets->max_idx);

            if(tail != NULL && tail->type.of == TYPE_CASH_JUMP && --skip == 0){
                Jump *origin = (Jump *)tail;
                origin->idx = jumpIdx;
                jmp->idx = temp.idx;
                return;
            }
        }
    }
}

Jump *Jump_Make(MemCh *m, cls typeOf){
    Jump *jmp = MemCh_AllocOf(m, sizeof(Jump), TYPE_CASH_JUMP);
    jmp->type.of = TYPE_CASH_JUMP;
    jmp->objType.of = typeOf;
    jmp->idx = -1;
    return jmp;
}
