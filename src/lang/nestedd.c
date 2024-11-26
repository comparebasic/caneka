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

static status nest(MemCtx *m, NestedD *nd, Span *t, status op){
    if(t == NULL || !Ifc_Match(t->type.of, TYPE_SPAN)){
        return ERROR;
    }

    if(op == NESTED_FOR){
        Iter_Init(&nd->it, t);
    }else if((op & NESTED_WITH) != 0 && t->type.of == TYPE_TABLE){
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
        nest(m, nd, (Span *)t, NESTED_WITH);
        return SUCCESS;
    }
    return MISS;
}

status NestedD_For(MemCtx *m, NestedD *nd, Abstract *key){
    Abstract *t =  Table_Get(nd->current_tbl, key);
    if(t != NULL && Ifc_Match(t->type.of, TYPE_SPAN)){
        nest(m, nd, (Span *)t, NESTED_FOR);
        return SUCCESS;
    }
    return MISS;
}

status NestedD_Outdent(MemCtx *m, NestedD *nd){
    Span_Cull(nd->stack, 1);

    NestedState *ns = Span_Get(nd->stack, nd->stack->max_idx);
    if(ns == NULL || ns->t == NULL){
        return ERROR;
    }

    return nest(m, nd, ns->t, (ns->flags|NESTED_OUTDENT));
}

Abstract *NestedD_Get(NestedD *nd, Abstract *key){
    if(nd->current_tbl == NULL){
        return NULL;
    }
    return Table_Get(nd->current_tbl, key);
}

status NestedD_Next(NestedD *nd){
    if(nd->it.values == NULL || (nd->it.type.state & END) != 0){
        return MISS;
    }
    Iter_Next(&nd->it);

    Abstract *next = Iter_Get(&nd->it);
    if(next->type.of == TYPE_TABLE){
        nd->current_tbl = (Span *)next;
        return SUCCESS;
    }else{
        nd->current_tbl = NULL;
        return MISS;
    }
}

NestedD *NestedD_Make(MemCtx *m, Span *tbl){
    NestedD *nd = MemCtx_Alloc(m, sizeof(NestedD));
    nd->type.of = TYPE_NESTEDD;

    nd->stack = Span_Make(m, TYPE_NESTED_SPAN);

    if(tbl != NULL){
        nd->current_tbl = tbl;
        nest(m, nd, tbl, NESTED_WITH);
    }

    return nd;
}
