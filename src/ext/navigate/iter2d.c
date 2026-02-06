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
        return;
    }else{
item:
        if(it->idx < 0 && (it->type.state & END)){
            it->type.state |= END;
            return;
        }

        Abstract *a = (Abstract *)Iter_Get(it);
        Table *children = NULL;
        if(a->type.of == TYPE_ITER){ 
            Iter *it2 = (Iter *)a;
            if(Iter_Next(it2) & END){
                Iter_Remove(it);
                Iter_Prev(it);

                void *args[] = {it, NULL};
                Out("^c.End @^0\n", args);
                exit(1);

                goto item;
            }else{
                a = Iter_Get(it2);
                if(a == NULL){
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Found null Item", NULL);
                }else if(a->type.of == TYPE_HASHED){
                    void *ar[] = {a, NULL};
                    Out("^b.Adding Above @^0\n", ar);
                    Iter_Push(it, ((Hashed *)a)->value);
                }else{
                    void *ar[] = {a, NULL};
                    Out("^b.Adding Above II @^0\n", ar);
                    Iter_Push(it, a);
                }
                return;
            }
        }else if(a != NULL && a->type.of & TYPE_INSTANCE &&
                !Empty((children =
                    Span_Get((Inst *)a, INST_PROPIDX_CHILDREN)))){
            Iter *it2 = Iter_Make(m, Table_Ordered(m, children));
            Iter_Push(it, it2);
            Iter_Next(it2);

            a = Iter_Get(it2);
            if(a != NULL && a->type.of == TYPE_HASHED){
                void *ar[] = {a, NULL};
                Out("^b.Adding Below @^0\n", ar);
                Iter_Push(it, ((Hashed *)a)->value);
            }else{
                Iter_Push(it, a);
            }
        }else{
            Out("^c.Remove Item^0\n", NULL);
            Iter_Remove(it);
            Iter_Prev(it);

            goto item;
        }
    }
}

void *Iter2d_Get(Iter *it){
    return Iter_Get(it);
}
