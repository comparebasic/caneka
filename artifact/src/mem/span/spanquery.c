#include <external.h>
#include <caneka.h>

SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim){
    if(dim > SPAN_MAX_DIMS){
        Fatal("Greater dim than available stack", TYPE_SPAN);
    }
    return sq->stack+dim;
}

void SpanQuery_Setup(SpanQuery *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SpanQuery));

    sr->type.of = TYPE_SPAN_QUERY;
    sr->op = op;
    sr->m = p->m;
    sr->span = p;
    sr->dims = p->dims;
    sr->idx = idx;
    sr->dimsNeeded = Span_GetDimNeeded(idx);

    return;
}

status Span_Query(SpanQuery *sr){
    MemCtx *m = sr->span->m;
    i32 idx = sr->idx;
    Span *p = sr->span;
    /* increase the dims with blank slabs if necessary */
    if(sr->dimsNeeded > sr->dims){
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < sr->dimsNeeded){
            slab *new_sl = Slab_Make(p->m);

            if(exp_sl == NULL){
                shelf_sl = sr->span->root;
                sr->span->root = new_sl;
            }else{
                void **ptr = (void *)exp_sl;
                *ptr = new_sl;
            }

            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void *)exp_sl;
        *ptr = shelf_sl;
    }
    sr->dims = p->dims;

    /* resize the span by adding dimensions and slabs as needed */
    byte dims = p->dims;
    SpanState *st = NULL;
    while(TRUE){
        /* make new if not exists */
        SpanQuery_SetStack(sr, dims, st);
        if(st->slab == NULL){
            if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
                return NOOP;
            }
            slab *new_sl = Slab_Make(p->m); 
            SpanState *prev = SpanQuery_StateByDim(sr, dims+1);
            void **ptr = (void **)prev->slab;
            ptr += prev->localIdx;
            *ptr = new_sl;
            st->slab = new_sl;
        }else{
            sr->queryDim = dims;
        }

        /* find or allocate a space for the new span */
        if(dims == 0){
            break;
        }

        dims--;
    }

    if(st == NULL){
        Fatal("Slab not found, SpanState is null\n", TYPE_SPAN);
    }

    if(st->localIdx >= SPAN_STRIDE){
        Fatal("localIdx greater than stride", p->type.of);
    }

    sr->type.state |= SUCCESS;
    return sr->type.state;

}
