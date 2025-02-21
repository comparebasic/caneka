#include <external.h>
#include <caneka.h>

int Span_AddOrdered(Span *p, Abstract *t){
    int idx = p->max_idx+1;
    int bIdx = p->max_idx;
    Abstract *b = Span_Get(p, bIdx);
    while(Abs_Cmp(t, b) > 0){
        Span_Set(p, idx, b);
        idx = bIdx;
        b = Span_Get(p, --bIdx);
    }
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }
    return -1;
}

void *Span_ReserveNext(Span *p){
    Reserve rsv;
    memset(&rsv, 0, sizeof(Reserve));
    rsv.type.of = TYPE_RESERVE;
    return Span_Set(p, Span_NextIdx(p), (Abstract *)&rsv);
}

status Span_Concat(Span *p, Span *add){
    status r = READY;
    Iter it;
    Iter_Init(&it, add);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a != NULL){
            r |= Span_Add(p, a);
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Span_Move(Span *p, int fromIdx, int toIdx){
    return NOOP;
}

Span *Span_From(MemCtx *m, int count, ...){
    Span *p = Span_Make(m, TYPE_SPAN);
    Abstract *v = NULL;
	va_list arg;
    va_start(arg, count);
    for(int i = 0; i < count; i++){
        v = va_arg(arg, AbstractPtr);
        Span_Add(p, v);
    }
    return p;
}

status Span_Run(MemCtx *m, Span *p, DoFunc func){
    status r = READY;
    for(int i = 0; i < p->nvalues; i++){
        Abstract *a = (Abstract *)Span_Get(p, i);
        if(a != NULL && (*(util *)a) != 0){
            r |= func(m, a);
        }
    }
    return r;
}

Span *Span_Clone(MemCtx *m, Span *op){
    Span *p = Span_Make(m, op->type.of);
    if((p->def->flags & SPAN_INLINE) != 0){
        p->def = SpanDef_Clone(m, SpanDef_FromCls(op->type.of));
        p->type.of = p->def->typeOf;
    }
    Iter it;
    Iter_Init(&it, op);
    while((Iter_Next(&it) & END) == 0){
        Abstract *o = Iter_Get(&it);
        if(o != NULL){
            Abstract *a = Clone(m, o); 
            if(a == NULL){
                return NULL;
            }
            Span_Set(p, it.idx, a);
        }
    }
    return p;
}
