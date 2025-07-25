#include <external.h>
#include <caneka.h>

static status Transp_Push(TranspCtx *ctx, Abstract *a){
    return Iter_Add(&ctx->it, a);
}

static i64 Transp_SetPrev(TranspCtx *ctx){
    Iter it;
    ctx->stackIdx = ctx->it.idx;
    Abstract *a = Iter_Current(&ctx->it);
    if(a != NULL && a->type.of == TYPE_NODE){
        Node *nd = (Node *)a;
        TranspFunc func = (TranspFunc)Lookup_Get(ctx->lk, nd->captureKey);
        if(func != NULL){
            return func(ctx, TRANSP_CLOSE);
        }
    }
    memcpy(&it, &ctx->it, sizeof(Iter));
    Abstract *current = Iter_Current(&it);
    if(current != NULL && current->type.of != TYPE_NODE){
        it.type.state = (it.type.state & NORMAL_FLAGS) | SPAN_OP_GET;
        while((Iter_Prev(&it) & END) == 0){
            Abstract *item = Iter_Current(&it);
            if(item != NULL && item->type.of == TYPE_NODE){
                Node *nd = (Node *)item;
                ctx->func = (TranspFunc)Lookup_Get(ctx->lk, nd->captureKey);
                break;
            }
        }
    }
    return 0;
}

i64 Transp(TranspCtx *ctx){
    DebugStack_Push(NULL, ZERO);

    i64 total = 0;
    if((ctx->it.type.state & END) && (ctx->type.state & (ERROR|NOOP)) == 0){
        ctx->type.state |= SUCCESS;
        DebugStack_Pop();
        return total;
    }

    Abstract *a = Iter_Current(&ctx->it);
    if(a == NULL){
        ctx->type.state |= ERROR;
        DebugStack_Pop();
        return total;
    }
    DebugStack_SetRef(a, a->type.of);
    if(a->type.of == TYPE_ITER){
        if((Iter_Next((Iter *)a) & END) == 0){
            Transp_Push(ctx, ((Iter*)a)->value);
            i64 total = Transp(ctx);
            DebugStack_Pop();
            return total;
        }else{
            Iter_PrevRemove(&ctx->it);
            total += Transp_SetPrev(ctx);
            i64 total = Transp(ctx);
            DebugStack_Pop();
            return total;
        }
    }else{
        if((i32)ctx->stackIdx == ctx->it.idx){ 
            /* skip to bottom */
        }else{
            if(a->type.of == TYPE_NODE){
                if(ctx->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)a,
                        NULL
                    };
                    Out("^y.Transp[a=TYPE_NODE](@)\n", args);
                }
                Node *na = (Node *)a;
                ctx->func = (TranspFunc)Lookup_Get(ctx->lk, na->captureKey);
                if(ctx->func != NULL){
                    total += ctx->func(ctx, TRANSP_OPEN);
                }

                if(na->child != NULL){
                    if(na->child->type.of == TYPE_NODE){
                        Transp_Push(ctx, na->child);
                        ctx->stackIdx = ctx->it.idx;
                    }else if(na->child->type.of == TYPE_SPAN){
                        Transp_Push(ctx, 
                            (Abstract *)Iter_Make(ctx->m, (Span *)na->child));
                    }else{
                        Transp_Push(ctx, na->child);
                        if(ctx->func != NULL){
                            total += ctx->func(ctx, TRANSP_BODY);
                        }
                    }
                }
            }else{
                if(ctx->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)a,
                        NULL
                    };
                    Out("^y.Transp[else a](@)\n", args);
                }
                a = Iter_Current(&ctx->it);
                if(ctx->func != NULL && a->type.of != TYPE_ITER){
                    total += ctx->func(ctx, TRANSP_BODY);
                }
            }
        }
    }

    a = Iter_Current(&ctx->it);
    if((i32)ctx->stackIdx == ctx->it.idx || 
            (a->type.of != TYPE_ITER || (a->type.state & LAST))){
        Iter_PrevRemove(&ctx->it);
        total += Transp_SetPrev(ctx);
    }
    
    if(ctx->it.type.state & END){
        ctx->type.state |= SUCCESS;
    }
    DebugStack_Pop();
    return total;
}

i64 Transp_Init(MemCh *m){
    status r = READY;
    r |= Transp_ToSInit(m, ToStreamLookup);
    r |= Transp_InitLabels(m, ToSFlagLookup);
    return r;
}
