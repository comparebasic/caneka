#include <external.h>
#include <caneka.h>
void Iter2d_InstInit(MemCh *m, Inst *inst, Iter *it){
    Iter_Init(it, Span_Make(m));
    Iter_Add(it, inst);
}

void Iter2d_InstNext(Iter *it){
    MemCh *m = it->p->m;
    if((it->type.state & PROCESSING) == 0){
        Iter_Next(it);
    }else{
item:
        if(it->idx < 0){
            it->type.state &= ~PROCESSING;
            it->type.state |= END;
            return;
        }

        Abstract *a = (Abstract *)Iter_Get(it);
        Table *children = NULL;
        if(a->type.of == TYPE_ITER){ 
            Iter *it2 = (Iter *)a;
loop:
            if(Iter_Next(it2) & END){
                Iter_Remove(it);
                Iter_Prev(it);

                goto item;
            }else{
                a = Iter_Get(it2);
                if(a == NULL){
                    goto loop;
                }else if(a->type.of == TYPE_HASHED){
                    Iter_Add(it, ((Hashed *)a)->value);
                }else{
                    Iter_Add(it, a);
                }
                return;
            }
        }else if(a != NULL && a->type.of & TYPE_INSTANCE &&
                !Empty((children =
                    Span_Get((Inst *)a, INST_PROPIDX_CHILDREN)))){
            Iter *it2 = Iter_Make(m, children);
            Iter_Add(it, it2);
            Iter_Next(it2);

            a = Iter_Get(it2);
            if(a != NULL && a->type.of == TYPE_HASHED){
                Iter_Add(it, ((Hashed *)a)->value);
            }else{
                Iter_Add(it, a);
            }
        }else{
            Iter_Remove(it);
            Iter_Prev(it);

            goto item;
        }
    }
}

void *Iter2d_Get(Iter *it){
    return Iter_Get(it);
}
