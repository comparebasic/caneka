#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

cls Persist_UnpackAddr(MemCh *pm, PersistCoord **coord){
    cls typeOf = coord->typeOf;
    MemPage *pg = Span_Get(pm->it.p, coord->idx);
    util u = (util)pg;
    u &= ~MEM_PERSIST_MASK;
    ((void *)(*coord)) = (void *)(u & coord->offset); 
    return typeOf;
}

status Persist_PackAddr(MemCh *m, Table *tbl, i32 slIdx, void **ptr, cls typeOf){
    util u = (util)ptr;
    u &= ~MEM_PERSIST_MASK;
    PersistItem *pageItem = Table_Get(tbl, Util_Wrapped(m, u));

    PersistCoord coord;
    coord.typeOf = typeOf;
    coord.idx = pageItem.idx;
    coord.offset = (quad)(((util)*ptr) & MEM_PERSIST_MASK);
    *ptr = (void *)coord;
    return SUCCESS;
}

status Persist_FlushFree(Stream *sm, MemCh *persist){
    status r = READY;
    SourceFunc func = NULL;
    Abstract *a = NULL;

    MemCh *m = sm->m;
    Table *tbl = Table_Make(m);
    Iter_Reset(&persist->it);
    while((Iter_Next(&persist->it) & END) == 0){
        Abstract *pg = Iter_Get(&persist->it);
        Table_Add(tbl, (Abstract *)Util_Wrapped(m, (util)pg), 
            PersistItem_Make(m, (void *)pg, pg->type.of));
    }

    Iter_Reset(&persist->it);
    while((Iter_Next(&persist->it) & END) == 0){
        MemPage *pg = Iter_Get(&persist->it);
        void *end = pg+PAGE_SIZE-1;
        void *ptr = ptr+((util)pg->remaining); 
        while(ptr <= end){
            Abstract *a = (Abstract *)ptr;
            Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
            if(map == NULL){
                /* Error */
            }
            for(i16 i = 1; i < map->type.range; i++){
                RangeType *att = map->atts+i;
                if(att->type.of > _TYPE_RAW_END){
                    Persist_PackAddr(m,
                        tbl,
                        persist->it.idx,
                        ((void *)a)+att->offset,
                        att->type.of);
                }
            }
            a->type.of = TYPE_BLANKED;
            Single *size = (Single *)Lookup_Get(TypeSizes, a->type.of);
            ptr += size->val.value;
        }
    }

    MemCh_Free(m);
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

