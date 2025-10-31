#include <external.h>
#include <caneka.h>

static status Transp_Push(TranspCtx *ctx, Abstract *a){
    if(ctx->type.state & DEBUG){
        Abstract *args[2];
        args[0] = (Abstract *)a;
        args[1] = NULL;
        Out("^y.  Transp_Push(a:&)^0\n", args);
    }
    return Iter_Add(&ctx->it, a);
}

static i64 Transp_SetPrev(TranspCtx *ctx){
    Iter it;
    ctx->stackIdx = ctx->it.idx;
    Abstract *a = Iter_Get(&ctx->it);
    if(a != NULL && a->type.of == TYPE_NODE){
        Node *nd = (Node *)a;
        TranspFunc func = (TranspFunc)Lookup_Get(ctx->lk, nd->captureKey);
        if(func != NULL){
            return func(ctx, TRANSP_CLOSE);
        }
    }
    memcpy(&it, &ctx->it, sizeof(Iter));
    Abstract *current = Iter_Get(&it);
    if(current != NULL && current->type.of != TYPE_NODE){
        it.type.state = (it.type.state & NORMAL_FLAGS) | SPAN_OP_GET;
        while((Iter_Prev(&it) & END) == 0){
            Abstract *item = Iter_Get(&it);
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
    Abstract *args[5];
    DebugStack_Push(NULL, ZERO);

    if(ctx->type.state & DEBUG){
        args[0] = (Abstract *)ctx;
        args[1] = NULL;
        Out("^y.>>> Transp(ctx:@)\n", args);
    }

    i64 total = 0;
    if((ctx->it.type.state & END) && (ctx->type.state & (ERROR|NOOP)) == 0){
        ctx->type.state |= SUCCESS;
        DebugStack_Pop();
        return total;
    }

    printf("Transp I\n");
    fflush(stdout);

    Abstract *a = Iter_Get(&ctx->it);
    if(a == NULL){
        ctx->type.state |= ERROR;
        args[0] = (Abstract *)&ctx->it;
        args[1] = NULL;
        Error(ctx->m, FUNCNAME, FILENAME, LINENUMBER,
            "Transp recieved NULL element from current iterator &", args);
        DebugStack_Pop();
        return total;
    }

    printf("Transp II\n");
    fflush(stdout);

    DebugStack_SetRef(a, a->type.of);
    if(a->type.of == TYPE_ITER){
        if((Iter_Next((Iter *)a) & END) == 0){
            printf("Push Iter\n");
            fflush(stdout);
            Transp_Push(ctx, ((Iter*)a)->value);
            i64 total = Transp(ctx);
            DebugStack_Pop();
            return total;
        }else{
            printf("Prev\n");
            fflush(stdout);

            Iter_Prev(&ctx->it);
            total += Transp_SetPrev(ctx);
            i64 total = Transp(ctx);
            DebugStack_Pop();
            return total;
        }
    }else{
        printf("Transp non-iter III\n");
        fflush(stdout);
        if((i32)ctx->stackIdx == ctx->it.idx){ 
            /* skip to bottom */
            printf("Transp skip to bottom IV\n");
            fflush(stdout);
        }else{
            printf("Transp content V\n");
            fflush(stdout);
            if(a->type.of == TYPE_NODE){
                if(ctx->type.state & DEBUG){
                    args[0] = (Abstract *)a;
                    args[1] = NULL;
                    Out("^y.Transp[a=TYPE_NODE](@)\n", args);
                }
                Node *na = (Node *)a;
                ctx->func = (TranspFunc)Lookup_Get(ctx->lk, na->captureKey);
                if(ctx->func != NULL){
                    total += ctx->func(ctx, TRANSP_OPEN);
                }

                if(na->child != NULL){
                    if(na->child->type.of == TYPE_NODE){
                        printf("  push node\n");
                        fflush(stdout);
                        Transp_Push(ctx, na->child);
                        ctx->stackIdx = ctx->it.idx;
                    }else if(na->child->type.of == TYPE_SPAN){
                        printf("  push span\n");
                        fflush(stdout);
                        Iter *it = Iter_Make(ctx->m, (Span *)na->child);
                        Iter_Next(it);
                        Transp_Push(ctx, (Abstract *)it);
                    }else{
                        printf("  push other\n");
                        fflush(stdout);
                        Transp_Push(ctx, na->child);
                        if(ctx->func != NULL){
                            total += ctx->func(ctx, TRANSP_BODY);
                        }
                    }
                }
            }else{
                if(ctx->type.state & DEBUG){
                    args[0] = (Abstract *)a;
                    args[1] = NULL;
                    Out("^y.Transp[else a](@)\n", args);
                }
                a = Iter_Get(&ctx->it);
                if(ctx->func != NULL && a->type.of != TYPE_ITER){
                    total += ctx->func(ctx, TRANSP_BODY);
                }
            }
        }
    }

    a = Iter_Get(&ctx->it);
    if((i32)ctx->stackIdx == ctx->it.idx || 
            (a->type.of != TYPE_ITER || (a->type.state & LAST))){
        Iter_PrevRemove(&ctx->it);
        total += Transp_SetPrev(ctx);
        args[0] = (Abstract *)ctx;
        args[1] = NULL;
        Out("^y.  Transp PrevRemove at bottom(ctx:@)\n", args);
    }
    
    if(ctx->it.type.state & END){
        ctx->type.state |= SUCCESS;
    }

    if(ctx->type.state & DEBUG){
        args[0] = (Abstract *)&ctx->it;
        args[1] = NULL;
        Out("^y.Transp it\\@Transp: @\n", args);
    }
    DebugStack_Pop();
    return total;
}
