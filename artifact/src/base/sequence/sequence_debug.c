#include <external.h>
#include <caneka.h>

i64 Iter_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    Str *s = State_ToStr(sm->m, it->type.state);
    void *args[] = {s, &it->idx, &it->span->nvalues, NULL};
    return StrVec_Fmt(sm, "I<_t:_i4 of _i4>", args);
}

i64 HKey_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    HKey *hk = (HKey *)as(a, TYPE_HKEY);
    void *args[] = {&hk->id, &hk->idx, &hk->dim, &hk->pos, NULL};
    return StrVec_Fmt(sm, "Hk<_b8/_b4 ^D._i1^d.dim ^D._i1^d.pos>", args);
}

i64 Slab_Print(struct stream *sm, slab *slab, i8 dim, i8 dims){
    /*
    printf("\x1b[%dm(%p){", color, slab);
    void **slot = (void **)slab;
    void **end = slot+SPAN_STRIDE;
    i32 i = 0;

    while(slot < end){
        if(*slot != NULL){
            if(slot != (void **)slab){
                printf(", ");
            }
            printf("\x1b[1;%dm%d\x1b[0;%dm=%p", color, i, color, *slot);
        }
        i++;
        slot++;
    }
    printf("}\x1b[0m");

    if(dim > 0){
        slot = (void **)slab;
        end = slot+SPAN_STRIDE;
        i = 0;
        while(slot < end){
            if(*slot != NULL){
                printf("\n");
                wsOut(m, v, dims-(dim-1));
                printf("\x1b[%dm{\n", color);
                wsOut(m, v, dims-(dim-1)+1);
                printf("%d=", i);
                Slab_Print(m, v, *slot, dim-1, dims, color); 
            }
            i++;
            slot++;
        }
        printf("\n");
    }

    printf("\x1b[%dm}\x1b[0m", color);
    */
    return 0;
}

i64 Lookup_Print(struct stream *sm, Abstract *a, cls type, boolean extended){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    /*
    printf("\x1b[%dm%sLk<offset:%d latest_idx:\%d", color, msg, lk->offset, lk->latest_idx);
    Debug_Print((void *)lk->values, 0, "", color, TRUE);
    printf("\x1b[%dm>\x1b[0m", color);
    */
    return 0;
}

status SequenceDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_HKEY, (void *)HKey_Print);
    return r;
}
