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

status Persist_PackAddr(MemCh *m, Table *tbl, i32 slIdx, void **ptr, cls typeOf){
    util u = (util)ptr;
    u &= ~MEM_PERSIST_MASK;
    PersistItem *pageItem = (PersistItem *)Table_Get(tbl, (Abstract *)Util_Wrapped(m, u));

    PersistCoord coord;
    PersistCoord_Fill(&coord, pageItem->coord.idx, *ptr, typeOf);
    memcpy(ptr, &coord, sizeof(void *));
    return SUCCESS;
}

status Persist_FlushFree(Stream *sm, MemCh *persist){
    status r = READY;
    SourceFunc func = NULL;
    Abstract *a = NULL;
    Abstract *args[5];

    MemCh *m = sm->m;
    Table *tbl = Table_Make(m);

    MemIter mit;
    MemIter_Init(&mit, persist);
    while((MemIter_Next(&mit) & END) == 0){
        Abstract *a = MemIter_Get(&mit);
        Table_Set(tbl, (Abstract *)Util_Wrapped(m, (util)a), 
            (Abstract *)PersistItem_Make(m, mit.slIdx, (void *)a, a->type.of));
    }

    args[0] = (Abstract *)tbl;
    args[1] = NULL;
    Out("^y.MemTable &\n", args);

    /*
    Iter_Reset(&persist->it);
    while((Iter_Next(&persist->it) & END) == 0){
        MemPage *pg = Iter_Get(&persist->it);
        void *end = pg+PAGE_SIZE-1;
        void *ptr = ptr+((util)pg->remaining); 
        while(ptr <= end){
            Abstract *a = (Abstract *)ptr;
            Map *map = (Map *)Lookup_Get(MapsLookup, a->type.of);
            if(map == NULL){
                Error(ErrStream->m, (Abstract *)persist, FUNCNAME, FILENAME, LINENUMBER,
                    "Map not found, needed for mem persist", NULL);
                return ERROR;
            }
            for(i16 i = 1; i < map->type.range; i++){
                RangeType *att = map->atts+i;
                if(att->of > _TYPE_RAW_END){
                    Persist_PackAddr(m,
                        tbl,
                        persist->it.idx,
                        ((void *)a)+att->range,
                        att->of);
                }
            }
            a->type.of = TYPE_BLANKED;
            Single *size = (Single *)Lookup_Get(SizeLookup, a->type.of);
            ptr += size->val.value;
        }
    }

    MemCh_Free(m);
    */
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
