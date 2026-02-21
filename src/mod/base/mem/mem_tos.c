#include <external.h>
#include "base_module.h"

static Str **iterLabels = NULL;
static Str **spanLabels = NULL;

static inline status wsOut(Buff *bf, i8 dim){
    while(dim-- > 0){
        return Buff_AddBytes(bf, (byte *)"    ", 4); 
    }
    return SUCCESS;
}

static boolean Span_Empty(void *_a){
    Span *p = (Span *)_a;
    return p->nvalues == 0;
}

static boolean Iter_Empty(void *_a){
    Iter *it = (Iter *)_a;
    if(it->type.of != TYPE_ITER){
        return FALSE;
    }
    return Span_Empty(it->p);
}

status Addr_ToS(Buff *bf, void *a, word flags){
    if(flags & DEBUG){
        Fmt(bf, "^D.", NULL);
    }
    if(a == NULL){
        Fmt(bf, "<*NULL>", NULL);
    }else{
        MemBook *book = MemBook_Get(a);
        util page = (util)(a-(void*)book);
        util local = page & PAGE_MASK; 
        page &= ~local;
        page = page / PAGE_SIZE;

        void *args[] = {
            I32_Wrapped(bf->m, 0),
            Util_Wrapped(bf->m, page),
            Util_Wrapped(bf->m, local),
            NULL
        };
        Fmt(bf, "<*$/$/$>", args);
    }
    return SUCCESS;
}

status MemCh_Print(Buff *bf, void *a, cls type, word flags){
    MemCh *m = (MemCh*)as(a, TYPE_MEMCTX); 
    void *args[5];
    args[0] = I64_Wrapped(bf->m, m->it.p->nvalues);
    args[1] = MemCount_Wrapped(bf->m,  MemCh_Used(m, 0));
    args[2] = NULL;


    Table *tbl = Table_Make(bf->m);

    if(flags & MORE){
        Fmt(bf, "MemCh<$pages ^D.$^d.used [", args);

        Iter it;
        Iter_Init(&it, m->it.p);
        while((Iter_Next(&it) & END) == 0){
            MemPage *sl = (MemPage *)Iter_Get(&it);
            args[0] = I32_Wrapped(bf->m, it.idx);
            args[1] = I16_Wrapped(bf->m, sl->remaining);
            args[2] = NULL;
            Fmt(bf, "Page#$/@remaining", args);
            if((it.type.state & LAST) == 0){
                Buff_AddBytes(bf, (byte *)", ", 2);
            }
        }
        
        MemIter mit;
        MemIter_Init(&mit, m);
        i32 idx = 0;
        i16 g = 0;
        while((MemIter_Next(&mit) & END) == 0){
            Guard_Incr(m, &g, 100, FUNCNAME, FILENAME, LINENUMBER);
            if((mit.type.state & MORE) == 0){
                Abstract *a = (Abstract *)MemIter_Get(&mit);
                Table_Set(tbl, Util_Wrapped(bf->m, (util)a), 
                    I32_Wrapped(bf->m, idx++));
            }else{
                idx = 0;
            }
        }

        MemIter_Init(&mit, m);
        g = 0;
        while((MemIter_Next(&mit) & END) == 0){
            Guard_Incr(m, &g, 100, FUNCNAME, FILENAME, LINENUMBER);
            if(mit.type.state & MORE){
                if(mit.slIdx > 0){
                    if(mit.type.state & LAST){
                        Buff_AddBytes(bf, (byte *)"]", 1);
                    }else{
                        Buff_AddBytes(bf, (byte *)"], ", 3);
                    }
                }
                args[0] = I32_Wrapped(bf->m, mit.slIdx);
                args[1] = NULL;
                Fmt(bf, "Page#$[", args);
            }else{
                Abstract *a = (Abstract *)MemIter_Get(&mit);
                Map *map = Lookup_Get(MapsLookup, a->type.of);
                Single *count = (Single *)Table_Get(tbl, 
                    Util_Wrapped(bf->m, (util)a));

                if(map != NULL){
                    if(count != NULL){
                        args[0] = count;
                        args[1] = map->keys[0];
                        args[2] = NULL;
                        Fmt(bf, "$:$", args);
                    }else{
                        args[0] = map->keys[0];
                        args[1] = NULL;
                        Fmt(bf, "$", args);
                    }
                    i32 max = (i32)((RangeType *)map)->range;
                    Buff_AddBytes(bf, (byte *)"(", 1);
                    boolean first = TRUE;
                    for(i32 i = 1; i <= max; i++){
                        RangeType *att = map->atts+i;
                        if(att->of > _TYPE_RAW_END){
                            if(!first){
                                Buff_AddBytes(bf, (byte *)", ", 2);
                            }
                            first = FALSE;
                            args[0] = map->keys[i];
                            void **dptr = ((void *)a)+att->range;
                            void *ptr = NULL;
                            if(dptr != NULL && *dptr != NULL){
                                ptr = *dptr;
                                if(att->of > _TYPE_RANGE_TYPE_START && att->of < _TYPE_RANGE_TYPE_END){
                                    ptr -= sizeof(RangeType);
                                }
                                Single *attCount = (Single *)Table_Get(tbl,
                                    Util_Wrapped(bf->m, (util)ptr));

                                Abstract *aa = (Abstract *)ptr;
                                Map *amap = Lookup_Get(MapsLookup, aa->type.of);
                                if(amap != NULL){
                                    args[1] = amap->keys[0];
                                }else{
                                    args[1] = Type_ToStr(bf->m, aa->type.of);
                                }
                                args[2] = (attCount != NULL ? 
                                    I16_Wrapped(bf->m, attCount->val.i) : Util_Wrapped(bf->m, (util)aa));

                                args[3] = NULL;
                                Fmt(bf, "$=$:$", args);
                            }
                        }
                    }
                    Buff_AddBytes(bf, (byte *)")", 1);
                }else{
                    if(count != NULL){
                        args[0] = count;
                        args[1] = Type_ToStr(bf->m, a->type.of);
                        args[2] = NULL;
                        Fmt(bf, "$:$", args);
                    }else{
                        args[0] = Type_ToStr(bf->m, a->type.of);
                        args[1] = NULL;
                        Fmt(bf, "$", args);
                    }
                }
                if((mit.type.state & (MORE|LAST)) != LAST){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
        }

         Buff_AddBytes(bf, (byte *)"]>", 2);
    }else{
        return  Fmt(bf, "MemCh<used:$>", args);
    }
    return SUCCESS;
}

status MemBook_Print(Buff *bf, void *a, cls type, word flags){
    MemBook *cp = (MemBook*)as(a, TYPE_BOOK); 
    return 0;
}

status Span_Print(Buff *bf, void *a, cls type, word flags){
    Span *p = (Span*)as(a, TYPE_SPAN); 

    if((flags & (MORE|DEBUG)) == (MORE|DEBUG)){
        void *args[] = {
            I32_Wrapped(bf->m, p->nvalues), 
            I32_Wrapped(bf->m, p->max_idx), 
            I8_Wrapped(bf->m, p->dims),
            NULL
        };
        Fmt(bf, "Span<^D.$^d.values/0..$/$dims [", args);
    }else if(flags & MORE){
        Buff_AddBytes(bf, (byte *)"[", 1);
    }
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = it.value;
        if(a != NULL){
            void *args[] = {
                I32_Wrapped(bf->m, it.idx),
                NULL
            };
            if(flags & DEBUG){
                Buff_AddBytes(bf, (byte *)"\n    ", 5);
            }
            if((flags & (MORE|DEBUG)) == (MORE|DEBUG)){
                Fmt(bf, "$:", args);
            }
            Abstract *item = (Abstract *)it.value;
            if(p->type.state & FLAG_SPAN_RAW){
                Bits_Print(bf, (byte *)&(it.value), sizeof(void *), MORE);
            }else if(item != NULL && item->type.of == TYPE_MEMSLAB){
                ToS(bf, it.value, 0, (flags & ~DEBUG));
            }else{
                ToS(bf, it.value, 0, flags);
            }
            if((it.type.state & LAST) == 0 && (flags & MORE)){
                Buff_AddBytes(bf, (byte *)",", 1);
            }
        }
    }

    if((flags & (MORE|DEBUG)) == (MORE|DEBUG)){
        Buff_AddBytes(bf, (byte *)"]>", 2);
    }else if(flags & MORE){
        Buff_AddBytes(bf, (byte *)"]", 1);
    }
    
    return SUCCESS;
}

status Iter_Print(Buff *bf, void *a, cls type, word flags){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(bf->m, it->type.of, it->type.state),
            I32_Wrapped(bf->m, it->idx),
            I32_Wrapped(bf->m, it->p->max_idx),
            I8_Wrapped(bf->m, it->p->dims),
            NULL
        };
        Fmt(bf, "I<@ $ of $max/$dims stack:\n", args);
        void *ptr = it->p->root;
        for(i8 i = it->p->dims; i >= 0; i--){
            if(it->stack[i] == NULL && (flags & (MORE|DEBUG))){
                void *args[] = {
                    I32_Wrapped(bf->m, i),
                    NULL
                };
                Fmt(bf, "  $: NULL\n", args);
            }else{
                i64 delta = 0;
                if(i > 0 && it->stack[i] != NULL){
                    delta = (it->stack[i] - ptr) / sizeof(void *);
                }
                void *args[] = {
                    I32_Wrapped(bf->m, i),
                    Ptr_Wrapped(bf->m, ptr, 0),
                    I32_Wrapped(bf->m, delta),
                    I32_Wrapped(bf->m, it->stackIdx[i]),
                    Ptr_Wrapped(bf->m, it->stack[i], 0), 
                    NULL
                };
                Fmt(bf, "  $: $+$/$ = @\n", args);
            }
            if(i > 0 && it->stack[i] != NULL){
                ptr = *((void **)it->stack[i]);
            }
        }
        word previous = bf->type.state;
        void *args2[] = {
            ((it->p->type.state & FLAG_SPAN_RAW) ?
                I64_Wrapped(bf->m, (i64)it->value) : it->value),
            NULL
        };
        Fmt(bf, "value=@>", args2);
    }else if(flags & MORE){
        void *args[] = {
            Type_StateVec(bf->m, it->type.of, it->type.state),
            I32_Wrapped(bf->m, it->idx),
            I32_Wrapped(bf->m, it->p->max_idx),
            it->value,
            NULL
        };
        Fmt(bf, "I<@ idx/max_idx=^D.$of$^d.\\@@>", args);
    }else{
        _ToStream_NotImpl(FUNCNAME, FILENAME, LINENUMBER, bf, a, type, flags);
    }

    return SUCCESS;
}

status Mem_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(iterLabels == NULL){
        iterLabels = (Str **)Arr_Make(m, 17);
        iterLabels[9] = Str_CstrRef(m, "GET");
        iterLabels[10] = Str_CstrRef(m, "SET");
        iterLabels[11] = Str_CstrRef(m, "REMOVE");
        iterLabels[12] = Str_CstrRef(m, "RESERVE");
        iterLabels[13] = Str_CstrRef(m, "RESIZE");
        iterLabels[14] = Str_CstrRef(m, "ADD");
        iterLabels[15] = Str_CstrRef(m, "LAST");
        iterLabels[16] = Str_CstrRef(m, "REVERSE");
        Lookup_Add(m, lk, TYPE_ITER, (void *)iterLabels);
        r |= SUCCESS;
    }
    return r;
}

status Mem_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCh_Print);
    r |= Lookup_Add(m, lk, TYPE_BOOK, (void *)MemBook_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);

    r |= Lookup_Add(m, EmptyLookup, TYPE_SPAN, (void *)Span_Empty);
    r |= Lookup_Add(m, EmptyLookup, TYPE_ITER, (void *)Iter_Empty);
    return r;
}
