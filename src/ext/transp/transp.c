#include <external.h>
#include <caneka.h>

static status Transp_Push(TranspCtx *ctx, void *_a){
    Abstract *a = (Abstract *)_a;
    DebugStack_Push(a, a->type.of);
    void *args[3];
    if(ctx->type.state & DEBUG){
        if(a->type.of == TYPE_ITER){
            args[0] = Type_ToStr(ctx->m, a->type.of);
            args[1] = I32_Wrapped(ctx->m, ((Iter *)a)->p->nvalues);
            args[2] = NULL;
            Out("^c.  Transp_Push(a->type.of:@/$nvalues)^0\n", args);
        }else{
            args[0] = Type_ToStr(ctx->m, a->type.of);
            args[1] = NULL;
            Out("^c.  Transp_Push(a->type.of:@)^0\n", args);
        }
    }
    ctx->stackIdx = ctx->it.idx;
    DebugStack_Pop();
    return Iter_Add(&ctx->it, a);
}

static i64 Transp_SetPrev(TranspCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    void *args[3];
    Iter_Remove(&ctx->it);
    Iter_Prev(&ctx->it);
    if(ctx->type.state & DEBUG){
        args[0] = I32_Wrapped(ctx->m, ctx->it.idx) ;
        args[1] = NULL;
        Out("^c.  Transp_SetPrev $^0\n", args);
    }
    ctx->stackIdx = ctx->it.idx;
    Abstract *a = (Abstract *)Iter_Get(&ctx->it);
    if(a != NULL && a->type.of == TYPE_NODE){
        Node *nd = (Node *)a;
        TranspFunc func = (TranspFunc)Lookup_Get(ctx->lk, nd->captureKey);
        if(func != NULL){
            if(ctx->type.state & DEBUG){
                Out("^c.     func(close)^0\n", NULL);
            }
            DebugStack_Pop();
            return func(ctx, TRANSP_CLOSE);
        }
    }
    Iter it;
    memcpy(&it, &ctx->it, sizeof(Iter));
    Abstract *current = (Abstract *)Iter_Get(&it);
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
    DebugStack_Pop();
    return 0;
}

i64 Transp(TranspCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    void *args[6];
    MemCh *m = ctx->m;

    if(ctx->type.state & DEBUG){
        args[0] = &ctx->it;
        args[1] = I32_Wrapped(m, ctx->stackIdx);
        args[2] = NULL;
        Out("^c.Transp(ctx->it:@/^D.$^d.stackIdx)^0\n", args);
    }

    i64 total = 0;
    if((ctx->it.type.state & END) && (ctx->type.state & (ERROR|NOOP)) == 0){
        ctx->type.state |= SUCCESS;
        if(ctx->type.state & DEBUG){
            args[0] = ctx;
            args[1] = NULL;
            Out("^y.   END|NOOP Transp(ctx:@)^0\n", args);
        }
        DebugStack_Pop();
        return total;
    }

    Abstract *a = (Abstract *)Iter_Get(&ctx->it);
    if(a == NULL){
        ctx->type.state |= ERROR;
        args[0] = &ctx->it;
        args[1] = NULL;
        Error(ctx->m, FUNCNAME, FILENAME, LINENUMBER,
            "Transp recieved NULL element from current iterator &", args);
        DebugStack_Pop();
        return total;
    }

    DebugStack_SetRef(a, a->type.of);
    if(a->type.of == TYPE_ITER){
        if((Iter_Next((Iter *)a) & END) == 0){
            Transp_Push(ctx, ((Iter*)a)->value);

            if(ctx->type.state & DEBUG){
                args[0] = ((Iter *)a)->value;
                args[1] = NULL;
                Out("^y.   < Iter/Push(a:@)^0\n", args);
            }

            i64 total = Transp(ctx);
            DebugStack_Pop();
            return total;
        }else{
            total += Transp_SetPrev(ctx);

            if(ctx->type.state & DEBUG){
                args[0] = a;
                args[1] = NULL;
                Out("^y.   < Iter/END(a:@)^0\n", args);
            }

            i64 total = Transp(ctx);
            DebugStack_Pop();
            return total;
        }
    }else{
        if(ctx->type.state & DEBUG){
            args[0] = a;
            args[1] = NULL;
            Out("^y.   > Non-iter(a:&)^0\n", args);
        }
        if((i32)ctx->stackIdx == ctx->it.idx){ 
            /* skip to bottom */
            if(ctx->type.state & DEBUG){
                args[0] = ctx;
                args[1] = NULL;
                Out("^y.   skip / stackIdx == it.idx: @^0\n", args);
            }
        }else{
            if(a->type.of == TYPE_NODE){
                if(ctx->type.state & DEBUG){
                    args[0] = a;
                    args[1] = NULL;
                    Out("^y.    a=TYPE_NODE](@)\n", args);
                }
                Node *na = (Node *)a;
                ctx->func = (TranspFunc)Lookup_Get(ctx->lk, na->captureKey);
                if(ctx->func != NULL){
                    if(ctx->type.state & DEBUG){
                        Out("^p.        func(open)\n^0", NULL);
                    }
                    total += ctx->func(ctx, TRANSP_OPEN);
                }

                if(na->child != NULL){
                    if(na->child->type.of == TYPE_NODE){
                        if(ctx->type.state & DEBUG){
                            args[0] = na;
                            args[1] = NULL;
                            Out("^y.      type:Node(na:@)^0\n", args);
                        }
                        Transp_Push(ctx, na->child);
                        ctx->stackIdx = ctx->it.idx;
                    }else if(na->child->type.of == TYPE_SPAN){
                        Iter *it = Iter_Make(ctx->m, (Span *)na->child);
                        if(ctx->type.state & DEBUG){
                            args[0] = it;
                            args[1] = NULL;
                            Out("^y.      type:span(it:@)^0\n", args);
                        }

                        Transp_Push(ctx, it);
                    }else{
                        if(ctx->type.state & DEBUG){
                            args[0] = na->child;
                            args[1] = NULL;
                            Out("^y.      type:/else(na->child:@)^0\n", args);
                        }

                        Transp_Push(ctx, na->child);
                        if(ctx->func != NULL){
                            if(ctx->type.state & DEBUG){
                                Out("^p.        func(body)\n^0", NULL);
                            }

                            total += ctx->func(ctx, TRANSP_BODY);
                        }
                    }
                }
            }else{
                a = Iter_Get(&ctx->it);
                if(ctx->type.state & DEBUG){
                    args[0] = a;
                    args[1] = NULL;
                    Out("^y.    else/non-node (a:@)\n", args);
                }
                if(ctx->func != NULL && a->type.of != TYPE_ITER){
                    if(ctx->type.state & DEBUG){
                        Out("^p.        func/below(body)\n^0", NULL);
                    }
                    total += ctx->func(ctx, TRANSP_BODY);
                }
            }
        }
    }

    a = Iter_Get(&ctx->it);
    if((i32)ctx->stackIdx == ctx->it.idx || (a->type.of != TYPE_ITER || (a->type.state & LAST))){
        if(ctx->type.of & DEBUG){
            args[0] = I32_Wrapped(m, ctx->stackIdx);
            args[1] = I32_Wrapped(m, ctx->it.idx);
            args[2] = a;
            args[3] = NULL;
            Out("^c. Transp remove at bottom(^D.$^d./^D.$^d. ctx->it->value: @)\n", args);
        }
        total += Transp_SetPrev(ctx);
    }
    
    if(ctx->it.type.state & END){
        ctx->type.state |= SUCCESS;
    }

    DebugStack_Pop();
    return total;
}
