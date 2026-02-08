#include <external.h>
#include <caneka.h>

void Iter2d_InstNext(Iter2d *it2d){
    MemCh *m = it2d->m;
    if((it2d->it.type.state & PROCESSING) == 0){
        Iter_Next(&it2d->it);
        return;
    }else{
item:
        if(it2d->it.idx < 0 || it2d->it.p->nvalues == 0){
            it2d->it.type.state |= END;
            return;
        }

        Abstract *a = (Abstract *)Iter_Get(&it2d->it);
        Table *children = NULL;
        if(a->type.of == TYPE_ITER){ 
            Iter *it2 = (Iter *)a;
            if(Iter_Next(it2) & END){
                Iter_Remove(&it2d->it);
                Iter_Prev(&it2d->it);

                goto item;
            }else{
                a = Iter_Get(it2);
                if(a == NULL){
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Found null Item", NULL);
                }else if(a->type.of == TYPE_HASHED){
                    Iter_Push(&it2d->it, ((Hashed *)a)->value);
                }else{
                    void *ar[] = {a, NULL};
                }
                return;
            }
        }else if(a != NULL && a->type.of & TYPE_INSTANCE &&
                !Empty((children =
                    Span_Get((Inst *)a, INST_PROPIDX_CHILDREN)))){
            Iter *it2 = Iter_Make(m, Table_Ordered(m, children));
            Iter_Set(&it2d->it, it2);

            Iter_Next(it2);
            a = Iter_Get(it2);
            if(a != NULL && a->type.of == TYPE_HASHED){
                Iter_Push(&it2d->it, ((Hashed *)a)->value);
            }else{
                Iter_Push(&it2d->it, a);
            }
        }else{
            Iter_Remove(&it2d->it);
            Iter_Prev(&it2d->it);

            goto item;
        }
    }
}

status Iter2d_State(Iter2d *it2d){
    return it2d->it.type.state;
}

void *Iter2d_Get(Iter2d *it2d){
    return Iter_Get(&it2d->it);
}

Span *Iter2d_GetCoord(Iter2d *it2d){
    return it2d->coord;
}

StrVec *Iter2d_GetPath(Iter2d *it2d){
    return it2d->path;
}

Iter2d *Iter2d_Make(MemCh *m, Inst *inst){
    Iter2d *it2d = MemCh_AllocOf(m, sizeof(Iter2d), TYPE_ITER2D);
    it2d->type.of = TYPE_ITER2D;
    it2d->m = m;
    it2d->coord = Span_Make(m);
    it2d->path = StrVec_Make(m);
    Iter_Init(&it2d->it, Span_Make(m));
    Iter_Add(&it2d->it, inst);

    return it2d;
}
