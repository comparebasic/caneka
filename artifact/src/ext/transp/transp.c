#include <external.h>
#include <caneka.h>

static status Transp_Push(TranspCtx *ctx, Abstract *a){
    Iter_Setup(&ctx->it, ctx->it.p, SPAN_OP_ADD, ctx->it.p->max_idx);
    ctx->it.value = (Abstract *)a;
    return Iter_Query(&ctx->it);
}

i64 Transp(TranspCtx *ctx){
    i64 total = 0;
    if(ctx->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)ctx,
            NULL
        };
        Debug("^p.&^0.\n", args);
    }
    if((ctx->it.type.state & END) && (ctx->type.state & (ERROR|NOOP)) == 0){
        ctx->type.state |= SUCCESS;
        return total;
    }

    Abstract *a = ctx->it.value;
    if(a == NULL){
        ctx->type.state |= ERROR;
        return total;
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
                ctx->func = (TranspFunc)Lookup_Get(ctx->lk, na->captureKey);
                if(ctx->func != NULL){
                    total += ctx->func(ctx, TRANSP_OPEN);
                }

                if(na->child != NULL){
                    if(na->child->type.of == TYPE_NODE){
                        Transp_Push(ctx, na->child);
                    }else if(na->child->type.of == TYPE_SPAN){
                        Transp_Push(ctx, 
                            (Abstract *)Iter_Make(ctx->m, (Span *)na->child));
                    }else{
                        Transp_Push(ctx, na->child);
                        total += ctx->func(ctx, TRANSP_BODY);
                    }
                }
            }
        }
    }

    if((a->type.state & PROCESSING) != 0 || 
            (a->type.of != TYPE_ITER || (a->type.state & LAST))){
        Iter_PrevRemove(&ctx->it);
        a = ctx->it.value;
        if(a->type.of == TYPE_NODE && 
                (ctx->func = (TranspFunc)Lookup_Get(ctx->lk, ((Node *)a)->captureKey))
                != NULL){
            total += ctx->func(ctx, TRANSP_CLOSE);
        }
    }
    
    if(ctx->it.type.state & END){
        ctx->type.state |= SUCCESS;
    }
    return total;
}

i64 Transp_Init(MemCh *m){
    status r = READY;
    r |= Transp_ToSInit(m, ToStreamLookup);
    return r;
}
