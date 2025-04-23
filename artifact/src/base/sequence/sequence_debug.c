#include <external.h>
#include <caneka.h>

i64 HKey_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    HKey *hk = (HKey *)as(a, TYPE_HKEY);
    void *args[] = {&hk->idx, &hk->idx, &hk->dim, &hk->pos, NULL};
    return StrVec_Fmt(sm, "HKey<_b4/_i4 ^D._i1^d.dim ^D._i1^d.pos>", args);
}

i64 Hashed_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(extended){
        void *args[] = {&h->idx, &h->id, h->item, h->value, NULL};
        return StrVec_Fmt(sm, "H<_i4 _b8/_d -> _D>", args);
    }else{
        void *args[] = {&h->idx, h->item, h->value, NULL};
        return StrVec_Fmt(sm, "H<_i4 _d -> _D>", args);
    }
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

i64 Lookup_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    i64 total = 0;
    total += StrVec_Fmt(sm, "Lk<", NULL);
    Iter it;
    Iter_Init(&it, lk->values);
    while((Iter_Next(&it) & END) == 0){
        if(it.value != NULL){
            char *sep = (it.type.state & FLAG_ITER_LAST) ? "" : ", ";
            i64 sepLen = (it.type.state & FLAG_ITER_LAST) ? 0 : 2;
            void *args[] = {&it.idx, it.value, sep, (void *)sepLen, NULL};
            total += StrVec_Fmt(sm, "_i4 -> _d_C", args);
        }
    }
    total += StrVec_Fmt(sm, ">", NULL);
    return total;
}

status SequenceDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_HKEY, (void *)HKey_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    return r;
}
