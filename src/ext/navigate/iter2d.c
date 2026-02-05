#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Iter2d_InstNext(Inst *inst, Iter *it){
    if((it->type.state & PROCESSING) == 0){
        Iter_Init(it, Span_Make(m));
        Iter_Add(it, inst);
        Iter_Next(&it);
    }else{
item:
        if(it->idx < 0){
            it->type.state &= ~PROCESSING;
            it->type.state |= END;
            return;
        }

        Abstract *a = (Abstract *)Iter_Get(&it);
        Table *children = NULL;
        if(a->type.of == TYPE_ITER){ 
            Iter *it2 = (Iter *)a;
            if(Iter_Next(it2) & END){
                Iter_Remove(&it);
                Iter_Prev(&it);

                goto item;
            }else{
                Iter_Add(it, Iter_Get(it2));
                return;
            }
        }else if(a != NULL && a->type.of & TYPE_INSTANCE &&
                    !Empty((children =
                        Span_Get((Inst *)inst, INST_PROPIDX_CHILDREN)))){
                Iter *it2 = Iter_Make(m);
                Iter_Init(&it2, children);
                Iter_Add(it, it2);
                Iter_Next(&it2);
                Iter_Add(it, Iter_Get(&it2));
            }else{
                Iter_Remove(&it);
                Iter_Prev(&it);

                goto item;
            }
        }
    }
}

void *Iter2d_Get(Iter *it){
    return Iter_Get(&it);
}

