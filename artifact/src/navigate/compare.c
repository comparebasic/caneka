#include <external.h>
#include <caneka.h>

status Compare_Push(Comp *comp, Abstract *a, Abstract *b){
    CompResult *cr = CompResult_Make(comp->m, a, b);
    Iter_Setup(&comp->it, comp->it.p, SPAN_OP_ADD, comp->it.p->max_idx);
    comp->it.value = (Abstract *)cr;
    return Iter_Query(&comp->it);
}

status Compare(Comp *comp){
    CompResult *cr = comp->it.value;
    if(cr == NULL){
        comp->type.state |= ERROR;
        return comp->type.state;
    }
    if(cr->a == NULL || cr->b == NULL || cr->a->type.of != cr->b->type.of){
        comp->type.state |= NOOP;
    }else{
        Abstract *a = cr->a;
        Abstract *b = cr->b;
        if(cr->a->type.of == TYPE_ITER &&
                (Iter_Next((Iter *)cr->a) & END) == 0 && (Iter_Next((Iter *)cr->b) & END) == 0){
            a = ((Iter*)cr->a)->value;
            b = ((Iter*)cr->b)->value;
        }

        if(a->type.of == TYPE_NODE){
            Node *na = (Node *)a;
            Node *nb = (Node *)b;
            if(na->typeOfChild != nb->typeOfChild){
                comp->type.state |= NOOP;
                if(comp->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)na,
                        (Abstract *)nb,
                        NULL
                    };
                    Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                        "typeOfChild mismatch $ vs $", args);
                }
                return comp->type.state;
            }
            if(na->value != NULL && na->value != nb->value){
                Compare_Push(comp, na->value, nb->value);
                if(Compare(comp) & (SUCCESS|ERROR|NOOP)){
                    if(comp->type.state & DEBUG){
                        Abstract *args[] = {
                            (Abstract *)na,
                            (Abstract *)nb,
                            NULL
                        };
                        Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                            "value mismatch $ vs $", args);
                    }
                    return comp->type.state;
                }
            }
            if(na->atts != NULL && na->atts != nb->atts){
                Compare_Push(comp, (Abstract *)na->atts, (Abstract *)nb->atts);
                if(Compare(comp) & (SUCCESS|ERROR|NOOP)){
                    if(comp->type.state & DEBUG){
                        Abstract *args[] = {
                            (Abstract *)na,
                            (Abstract *)nb,
                            NULL
                        };
                        Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                            "atts mismatch $ vs $", args);
                    }
                    return comp->type.state;
                }
            }
            if(na->child != NULL && na->child != nb->child){
                if(nb->type.of == TYPE_SPAN){
                    Compare_Push(comp,
                        (Abstract *)Iter_Make(comp->m, (Span *)na->child), 
                        (Abstract *)Iter_Make(comp->m, (Span *)nb->child));
                }else{
                    Compare_Push(comp,
                        (Abstract *)na->child, 
                        (Abstract *)nb->child);
                }
                Compare(comp);
            }
        }else{
            if(!Equals(a, b)){
                if(comp->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)comp,
                        NULL
                    };
                    Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                        "mismatch $", args);
                }
                comp->type.state |= NOOP;
            }
        }
    }
    if(cr->a->type.of != TYPE_ITER || (cr->a->type.state & FLAG_ITER_LAST)){
        comp->it.type.state = (comp->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET;
        Iter_Prev(&comp->it);
    }
    if((comp->it.type.state & END) && (comp->type.state & (ERROR|NOOP)) == 0){
        comp->type.state |= SUCCESS;
    }
    return comp->type.state;
}

CompResult *CompResult_Make(MemCh *m, Abstract *a, Abstract *b){
    CompResult *cr = (CompResult *)MemCh_Alloc(m, sizeof(CompResult));
    cr->type.of = TYPE_COMPRESULT;
    if(a == NULL && a == b){
        cr->type.state |= SUCCESS;
    }else{
        if(a == NULL || b == NULL){
            cr->type.state |= ERROR;
        }
        cr->a = a;
        cr->b = b;
    }
    return cr;
}

Comp *Comp_Make(MemCh *m, Abstract *a, Abstract *b){
    Comp *comp = (Comp *)MemCh_Alloc(m, sizeof(Comp));
    comp->type.of = TYPE_COMP;
    comp->m = m;
    CompResult *cr = CompResult_Make(m, a, b);
    if(cr->type.state & (SUCCESS|ERROR)){
        comp->type.state |= (cr->type.state & (SUCCESS|ERROR));
        return comp;
    }

    Iter_Setup(&comp->it, Span_Make(m), SPAN_OP_SET, 0);
    comp->it.value = (Abstract *)cr;
    Iter_Query(&comp->it);
    return comp;
}
