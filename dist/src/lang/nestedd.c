#include <external.h> 
#include <caneka.h> 

char *NestedD_opToChars(status op){
   if((op & NESTED_OUTDENT) != 0){
        return "NESTED_OUTDENT";
   }else if(op == NESTED_FOR){
        return "NESTED_FOR";
   }else if(op == NESTED_WITH){
        return "NESTED_WITH";
   }else{
        return "unknown";
   } 
}

static status nest(NestedD *nd, Span *t, status op){
    MemCtx *m = nd->m;
    if(t == NULL || !Ifc_Match(t->type.of, TYPE_SPAN)){
        return ERROR;
    }

    if((op & NESTED_FOR) != 0){
        Iter_Init(&nd->it, t);
        nd->current_tbl = NULL;
    }else if((op & NESTED_WITH) != 0 && t->type.of == TYPE_TABLE){
        memset(&nd->it, 0, sizeof(Iter));
        nd->current_tbl = t;
    }else if((op & NESTED_OUTDENT) == 0){
        nd->type.state |= ERROR;
        return nd->type.state;
    }

    if((op & NESTED_OUTDENT) == 0){
        NestedState ns;
        ns.t = t;
        ns.idx = 0;
        ns.flags = op;
        ns.prev = Span_Get(nd->stack, nd->stack->max_idx);
        if(ns.prev != NULL){
            ns.prev->idx = nd->it.idx;
        }
        Span_Add(nd->stack, (Abstract *)&ns);
    }

    nd->type.state = (op & ~NESTED_OUTDENT);

    if(DEBUG_NESTED){
        printf("\x1b[%dmafter %s: \n", DEBUG_NESTED, NestedD_opToChars(op));
        Debug_Print((void *)nd, 0, "nest: ", DEBUG_NESTED, TRUE);
        printf("\n");
    }


    return SUCCESS;
}

status NestedD_With(MemCtx *m, NestedD *nd, Abstract *key){
    Abstract *t =  Table_Get(nd->current_tbl, key);
    if(t != NULL && t->type.of == TYPE_TABLE){
        nest(nd, (Span *)t, NESTED_WITH);
        return SUCCESS;
    }
    return NOOP;
}

status NestedD_For(NestedD *nd, Abstract *key){
    Abstract *t =  Table_Get(nd->current_tbl, key);
    if(t != NULL && Ifc_Match(t->type.of, TYPE_SPAN)){
        nest(nd, (Span *)t, NESTED_FOR);
        return SUCCESS;
    }
    return NOOP;
}

status NestedD_Outdent(NestedD *nd){
    Span_Cull(nd->stack, 1);

    NestedState *ns = Span_Get(nd->stack, nd->stack->max_idx);
    if(ns == NULL || ns->t == NULL){
        return ERROR;
    }

    status r = nest(nd, ns->t, (ns->flags|NESTED_OUTDENT));
    if((nd->type.state & NESTED_FOR) != 0){
        nd->it.idx = ns->idx;
    }
    return r;
}

Abstract *NestedD_Get(NestedD *nd, Abstract *key){
    if(nd->current_tbl == NULL){
        return NULL;
    }
    return Table_Get(nd->current_tbl, key);
}

status NestedD_Next(NestedD *nd){
    if(nd->it.values == NULL || (Iter_Next(&nd->it) & END) != 0){
        return END;
    }
    if((nd->type.state & NESTED_FOR) != 0){
        Abstract *next = Iter_Get(&nd->it);
        if(next->type.of == TYPE_TABLE){
            nd->current_tbl = (Span *)next;
            return SUCCESS;
        }else{
            nd->current_tbl = NULL;
            return END;
        }
    }
    return NOOP;
}

status NestedD_Init(MemCtx *m, NestedD *nd, Span *tbl){
    nd->type.state = ZERO;
    nd->stack = Span_Make(m, TYPE_NESTED_SPAN);
    if(tbl != NULL){
        nd->current_tbl = tbl;
        nest(nd, tbl, NESTED_WITH);
        return SUCCESS;
    }
    return NOOP;
}

NestedD *NestedD_Make(MemCtx *m, Span *tbl){
    NestedD *nd = MemCtx_Alloc(m, sizeof(NestedD));
    nd->type.of = TYPE_NESTEDD;
    nd->m = m;

    NestedD_Init(m, nd, tbl);

    return nd;
}
