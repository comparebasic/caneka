#include <external.h>
#include <caneka.h>

status Compare_Push(Comp *comp, Abstract *a, Abstract *b){
    CompResult *cr = CompResult_Make(comp->m, a, b);
    return Iter_Add(&comp->it, (Abstract *)cr);
}

status Compare(Comp *comp){
    if((comp->it.type.state & END) && (comp->type.state & (ERROR|NOOP)) == 0){
        comp->type.state |= SUCCESS;
        return comp->type.state;
    }

    CompResult *cr = Iter_Current(&comp->it);
    if(cr == NULL){
        comp->type.state |= ERROR;
        return comp->type.state;
    }
    if(cr->a == NULL || cr->b == NULL || cr->a->type.of != cr->b->type.of){
        comp->type.state |= NOOP;
    }else{
        Abstract *a = cr->a;
        Abstract *b = cr->b;
        if(a->type.of == TYPE_ITER){
            if(((Iter_Next((Iter *)cr->a) & END) | (Iter_Next((Iter *)cr->b) & END))
                    == 0){
                a = ((Iter*)cr->a)->value;
                b = ((Iter*)cr->b)->value;
            }else if((cr->a->type.state & END) == (cr->b->type.state & END)){
                comp->it.type.state = (comp->it.type.state & NORMAL_FLAGS) | (SPAN_OP_GET|SPAN_OP_REMOVE);
                Iter_Prev(&comp->it);
                comp->it.type.state &= ~SPAN_OP_REMOVE;
                return Compare(comp);
            }else{
                comp->type.state |= NOOP;
                if(comp->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)comp,
                        (Abstract *)cr->a,
                        (Abstract *)cr->b,
                        NULL
                    };
                    Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                        "iter END mismatch & -^E. @ vs @^e.", args);
                }
                return comp->type.state;
            }
        }else{
            if((cr->type.state & PROCESSING) != 0){
                /* skip to bottom */
            }else{
                cr->type.state |= PROCESSING;
                if(a->type.of == TYPE_NODE){
                    Node *na = (Node *)a;
                    Node *nb = (Node *)b;
                    if(na->typeOfChild != nb->typeOfChild){
                        comp->type.state |= NOOP;
                        if(comp->type.state & DEBUG){
                            Abstract *args[] = {
                                (Abstract *)Type_ToStr(comp->m, na->typeOfChild),
                                (Abstract *)Type_ToStr(comp->m, nb->typeOfChild),
                                (Abstract *)comp,
                                NULL
                            };
                            Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                                "typeOfChild mismatch $ vs $ &", args);
                        }
                        return comp->type.state;
                    }
                    if(na->value != NULL && na->value != nb->value){
                        Compare_Push(comp, na->value, nb->value);
                        if(Compare(comp) & (SUCCESS|ERROR|NOOP)){
                            if(comp->type.state & DEBUG){
                                Abstract *args[] = {
                                    (Abstract *)comp,
                                    NULL
                                };
                                Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                                    "value mismatch &", args);
                            }
                            return comp->type.state;
                        }
                    }
                    if(na->atts != NULL && na->atts != nb->atts){
                        Compare_Push(comp, (Abstract *)na->atts, (Abstract *)nb->atts);
                        if(Compare(comp) & (SUCCESS|ERROR|NOOP)){
                            if(comp->type.state & DEBUG){
                                Abstract *args[] = {
                                    (Abstract *)comp,
                                    NULL
                                };
                                Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                                    "atts mismatch &", args);
                            }
                            return comp->type.state;
                        }
                    }else if(nb->atts != NULL){
                        if(comp->type.state & DEBUG){
                            Abstract *args[] = {
                                (Abstract *)comp,
                                NULL
                            };
                            Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                                "atts of B mismatch &", args);
                        }
                        comp->type.state |= NOOP;
                        return comp->type.state;
                    }
                    if(na->child != NULL && na->child != nb->child){
                        if(nb->child->type.of == TYPE_RELATION){
                            Relation *ra = (Relation *)na->child;
                            Relation *rb = (Relation *)nb->child;
                            Iter_Setup(&ra->it, ra->it.p, SPAN_OP_GET, 0);
                            Iter_Setup(&rb->it, rb->it.p, SPAN_OP_GET, 0);
                            Compare_Push(comp,
                                (Abstract *)&ra->it, 
                                (Abstract *)&rb->it);
                        }else if(nb->child->type.of == TYPE_SPAN){
                            Compare_Push(comp,
                                (Abstract *)Iter_Make(comp->m, (Span *)na->child), 
                                (Abstract *)Iter_Make(comp->m, (Span *)na->child));
                        }else{
                            Compare_Push(comp,
                                (Abstract *)na->child, 
                                (Abstract *)nb->child);
                        }
                        Compare(comp);
                    }
                }else{
                    a->type.state |= DEBUG;
                    if(!Equals(a, b)){
                        if(comp->type.state & DEBUG){
                            Abstract *args[] = {
                                (Abstract *)comp,
                                (Abstract *)a,
                                (Abstract *)b,
                                NULL
                            };
                            Error(comp->m, (Abstract *)comp, FILENAME, FUNCNAME, LINENUMBER,
                                "mismatch & - ^p.&^r. vs ^p.&^r.", args);
                        }
                        comp->type.state |= NOOP;
                    }
                }
            }
        }
    }
    if((cr->type.state & PROCESSING) != 0 || 
            (cr->a->type.of != TYPE_ITER || (cr->a->type.state & LAST))){
        comp->it.type.state = (comp->it.type.state & NORMAL_FLAGS) | (SPAN_OP_GET|SPAN_OP_REMOVE);
        Iter_Prev(&comp->it);
        comp->it.type.state &= ~SPAN_OP_REMOVE;
    }
    if(comp->it.type.state & END){
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

    Iter_Init(&comp->it, Span_Make(m));
    Iter_Add(&comp->it, (Abstract *)cr);
    return comp;
}
