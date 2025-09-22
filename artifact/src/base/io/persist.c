#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

cls Persist_RepointAddr(MemCh *pm, void **ptr){
    PersistCoord *coord = (PersistCoord *)ptr;
    cls typeOf = coord->typeOf;
    MemPage *pg = Span_Get(pm->it.p, coord->idx);
    util u = (util)pg;
    u &= ~MEM_PERSIST_MASK;
    *ptr = (void *)(u & coord->offset); 
    return typeOf;
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

PersistItem *PersistItem_Make(MemCh *m, i32 slIdx, void *ptr, cls typeOf){
    PersistItem *item = MemCh_Alloc(m, sizeof(PersistItem));
    item->ptr = ptr;
    item->coord.typeOf = typeOf;
    item->coord.idx = slIdx;
    item->coord.offset= (quad)(((util)ptr) & MEM_PERSIST_MASK);
    return item;
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
            if(a->type.of != TYPE_BLANKED && 
                    (func = (SourceFunc)Lookup_Get(BlankerLookup, a->type.of)) != NULL){
                r |= func(m, a, (Abstract *)tbl);
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
