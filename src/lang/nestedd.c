#include <basictypes_external.h> 
#include <caneka/caneka.h> 

static lifecycle_t nest(MemCtx *m, NestedD *nd, Typed *t, lifecycle_t nestState, cbool_t stack){
    if(t->type != TYPE_SPAN){
        nd->nestState = 0;
        return ERROR;
    }

    if(nestState == NESTED_FOR){
        Iter *it = Iter_Make(m, t);
        nd->current_it = it;
        NestedD_Next(m, nd);
        if(stack){
            Span_Add(m, nd->stack_sp, Typed_Make(m, nestState));
            Span_Add(m, nd->stack_sp, (Typed *)it);
        }
    }else if(nestState == NESTED_WITH && Table_IsTable(t) == SUCCESS){
        if(stack){
            Span_Add(m, nd->stack_sp, Typed_Make(m, nestState));
            Span_Add(m, nd->stack_sp, (Typed *)t);
        }
        nd->current_tbl = (Span *)t;
    }else{
        nd->nestState = 0;
        return ERROR;
    }

    nd->nestState = nestState;

    return SUCCESS;
}

lifecycle_t NestedD_With(MemCtx *m, NestedD *nd, Typed *key){
    Typed *t =  Table_Get(m, nd->current_tbl, key);
    if(t != NULL && Table_IsTable(t)){
        nest(m, nd, t, NESTED_WITH, true);
        return SUCCESS;
    }
    return MISS;
}

lifecycle_t NestedD_For(MemCtx *m, NestedD *nd, Typed *key){
    Typed *t =  Table_Get(m, nd->current_tbl, key);
    if(t != NULL && t->type == TYPE_SPAN){
        nest(m, nd, t, NESTED_FOR, true);
        return SUCCESS;
    }
    return MISS;
}

lifecycle_t NestedD_Outdent(MemCtx *m, NestedD *nd){
    Span_Cull(m, nd->stack_sp, 2);
    Typed *stateT = Span_Get(m, nd->stack_sp, -2); 
    Span *data_tbl = (Span *)Span_Get(m, nd->stack_sp, -1); 
    if(stateT == NULL || data_tbl == NULL){
        return ERROR;
    }
    return nest(m, nd, (Typed *)data_tbl, stateT->lifecycle, false);
}

Typed *NestedD_Get(MemCtx *m, NestedD *nd, Typed *key){
    if(nd->current_tbl == NULL){
        return NULL;
    }
    return Table_Get(m, nd->current_tbl, key);
}

lifecycle_t NestedD_Next(MemCtx *m, NestedD *nd){
    if(nd->current_it == NULL || IsDone(nd->current_it)){
        return MISS;
    }
    Typed *next = Iter_Next(nd->current_it);
    if(Table_IsTable(next)){
        nd->current_tbl = (Span *)next;
        return SUCCESS;
    }else{
        nd->current_tbl = NULL;
        return MISS;
    }
}

NestedD *NestedD_New(MemCtx *m){
    NestedD *nd = Alloc(m, sizeof(NestedD));
    nd->type = TYPE_NESTED_DATA;
    nd->stack_sp = Span_Make(m, SPAN_DIM_SIZE);

    return nd;
}

NestedD *NestedD_Make(MemCtx *m, Span *data_tbl){
    NestedD *nd = NestedD_New(m);
    nd->root_tbl = data_tbl;
    nd->current_tbl = data_tbl;
    nest(m, nd, (Typed *)data_tbl, NESTED_WITH, true);

    return nd;
}
