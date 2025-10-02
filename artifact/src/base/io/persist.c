#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

cls Persist_UnpackAddr(MemCh *pm, void **ptr){
    PersistCoord *coord = *ptr;
    cls typeOf = coord->typeOf;
    MemPage *pg = Span_Get(pm->it.p, coord->idx);
    util u = (util)pg;
    u &= ~MEM_PERSIST_MASK;
    *ptr = (void *)(u & coord->offset); 
    return typeOf;
}

status Persist_PackAddr(cls typeOf, i32 slIdx, void **ptr){
    util u = (util)ptr;
    u &= ~MEM_PERSIST_MASK;

    PersistCoord coord = {
       .typeOf = typeOf,
       .idx = slIdx,
       .offset = (quad)u,
    };
    memcpy(ptr, &coord, sizeof(void *));
    return SUCCESS;
}

status Persist_FlushFree(Stream *sm, MemCh *persist){
    status r = READY;
    SourceFunc func = NULL;
    Abstract *a = NULL;
    Abstract *args[5];
    Iter it;

    MemCh *m = sm->m;
    Table *tbl = Table_Make(m);

    MemIter mit;
    MemIter_Init(&mit, persist);
    while((MemIter_Next(&mit) & END) == 0){
        Abstract *a = MemIter_Get(&mit);
        if((mit.type.state & MORE) == 0){
            Table_Set(tbl, (Abstract *)Util_Wrapped(m, (util)a), 
                (Abstract *)PersistItem_Make(m, mit.slIdx, (void *)a, a->type.of));
        }
    }

    Span *pages = Span_CloneShallow(m, persist->it.p);

    MemIter_Init(&mit, persist);
    while((MemIter_Next(&mit) & END) == 0){
        Abstract *a = MemIter_Get(&mit);
        if((mit.type.state & MORE) == 0){
            if(a->type.of == TYPE_POINTER_ARRAY){
                void *ptr = ((void *)a)+sizeof(RangeType);
                i32 slots = ((RangeType *)a)->range / sizeof(void *);
                void *end = ptr+slots-1;
                while(ptr <= end){
                    PersistItem *item = (PersistItem *)Table_Get(tbl, 
                        (Abstract *)Util_Wrapped(m, (util)a));
                    if(item != NULL){
                        Persist_PackAddr(item->coord.typeOf, mit.slIdx, (void **)&ptr);
                    }else{
                        Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                            "Unable to find address in table, may be external to this MemCh", NULL);
                    }
                    ptr++;
                }
            }else if(a->type.of > _TYPE_ABSTRACT_BEGIN){
                if(a->type.of == TYPE_MEMCTX){
                    continue;
                }else{
                    Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
                    if(map == NULL){
                        args[0] = (Abstract *)Type_ToStr(m, a->type.of);
                        args[1] = NULL;
                        Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                            "Map not found for type $, needed for mem persist", args);
                        return ERROR;
                    }
                    if(a->type.of == TYPE_ITER){
                        Iter *itp = (Iter *)a;
                        Iter_Init(itp, itp->p);
                    }
                    for(i16 i = 1; i <= map->type.range; i++){
                        RangeType *att = map->atts+i;
                        if(att->of > _TYPE_RANGE_TYPE_START){
                            Abstract *aa = ((void *)a)+att->range;
                            PersistItem *item = (PersistItem *)Table_Get(tbl, 
                                (Abstract *)Util_Wrapped(m, (util)a));
                            if(item != NULL){
                                Persist_PackAddr(item->coord.typeOf, item->coord.idx, (void **)&aa);
                            }else{
                                Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                                    "Unable to find address in table, may be external to this MemCh", NULL);
                            }
                        }
                    }
                }
            }
        }
    }

    Iter_Init(&it, pages);
    while((Iter_Next(&it) & END) == 0){
        /* TODO: write blanked pages to stream */
        r |= MemBook_FreePage(m, (MemPage *)Iter_Get(&it));
    }

    return r;
}

status Persist_FromStream(MemCh *m, Stream *sm){
    status r = READY;
    return r;
}

status Persist_Init(MemCh *m){
    status r = READY;
    if(!_initialized){
        _initialized = TRUE;
        BlankerLookup = Lookup_Make(m, 0);
        RepointerLookup = Lookup_Make(m, 0);
        r |= SUCCESS;
    }

    if(r == READY){
        r = NOOP;
    }

    return r;
}
