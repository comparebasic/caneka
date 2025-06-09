#include <external.h>
#include <caneka.h>

status Transp_Push(TranspCtx *ctx, Abstract *a){
    Iter_Setup(&comp->it, comp->it.p, SPAN_OP_ADD, comp->it.p->max_idx);
    comp->it.value = (Abstract *)a;
    return Iter_Query(&comp->it);
}

status Transp(TranspCtx *ctx){
    i64 total = 0;
    if((ctx->it.type.state & END) && (ctx->type.state & (ERROR|NOOP)) == 0){
        ctx->type.state |= SUCCESS;
        return ctx->type.state;
    }

    Abstract *a = ctx->it.value;
    if(a == NULL){
        ctx->type.state |= ERROR;
        return ctx->type.state;
    }
    if(a->type.of == TYPE_ITER){
        if((Iter_Next((Iter *)a) & END) == 0){
            Transp_Push(ctx, ((Iter*)a)->value);
            return Transp(ctx);
        }else{
            ctx->it.type.state = (ctx->it.type.state & NORMAL_FLAGS) | (SPAN_OP_GET|SPAN_OP_REMOVE);
            Iter_Prev(&ctx->it);
            ctx->it.type.state &= ~SPAN_OP_REMOVE;
            return Transp(ctx);
        }
    }else{
        if((a->type.state & PROCESSING) != 0){
            /* skip to bottom */
        }else{
            a->type.state |= PROCESSING;
            if(a->type.of == TYPE_NODE){
                Node *na = (Node *)a;
                TranspFunc func = (TranspFunc)Lookup_Get(ctx->lk, na->captureKey);
                if(func != NULL){
                    total += func(ctx);
                }

                if(na->child != NULL){
                    if(na->child->type.of == TYPE_NODE){
                        Transp_Push(ctx, na->child);
                    }else if(nb->child->type.of == TYPE_SPAN){
                        Transp_Push(ctx, 
                            (Abstract *)Iter_Make(ctx->m, (Span *)na->child));
                    }
                    Transp(ctx);
                }
            }
        }
    }
    if((a->type.state & PROCESSING) != 0 || 
            (a->type.of != TYPE_ITER || (a->type.state & FLAG_ITER_LAST))){
        ctx->it.type.state = (ctx->it.type.state & NORMAL_FLAGS) | (SPAN_OP_GET|SPAN_OP_REMOVE);
        Iter_Prev(&ctx->it);
        ctx->it.type.state &= ~SPAN_OP_REMOVE;
    }
    
    if(ctx->it.type.state & END){
        ctx->type.state |= SUCCESS;
    }
    return total;
}

