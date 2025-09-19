#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

status Persist_FillRef(MemCh *m, void **ptr){
    i32 idx = 0;
    PersistItem *item = Persist_GetArray(m);
    while(item != NULL && idx < PERSIST_ARR_MAX){
        if(*ptr == item->ptr){
            memcpy(*ptr, &item->coord, sizeof(void *));
        }
    }
    return NOOP;
}

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

Persist *Persist_Get(MemCh *m){
    if(m->type.of != TYPE_PERSIST_MEMCTX){
        Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
            "Mismatch of type when trying to get the span of a persist memchapter.", NULL);
        return NULL;
    }
    Iter it;
    Iter_Init(&it, m->it.p);
    void *ptr = NULL;
    while((Iter_Next(&it) & END) == 0){
        MemPage *pg = Iter_Get(&it);
        if(pg->type.state & MEM_PAGE_PERSIST_ARRAY){
            ptr = (void *)pg;
        }
    }
    if(ptr == NULL){
        Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find Persist MemPage.", NULL);
        return NULL;
    }
    return (Persist *)ptr+sizeof(MemPage);
}

PersistItem *Persist_GetArray(MemCh *m){
    Persist *pst = Persist_Get(m);
    return ((void *)pst)+sizeof(Persist);
}

status Persist_SetRef(MemCh *m, i32 slIdx, MemPage *pg, void *ptr){
    printf("set ref\n");
    PersistItem item;
    item.ptr = ptr;
    item.coord.typeOf = ZERO;
    item.coord.idx = slIdx;
    item.coord.offset= (quad)(((util)ptr) & MEM_PERSIST_MASK);
    Persist *pt = Persist_Get(m);
    PersistItem **arr = ((void *)pt)+sizeof(Persist);
    if(pt->maxIdx > PERSIST_ARR_MAX){
        Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
            "Persist array out of space", NULL);
        return ERROR;
    }
    memcpy(arr+pt->maxIdx, &item, sizeof(PersistItem));
    pt->maxIdx++;
    return SUCCESS;
}

status Persist_FlushFree(Stream *sm, MemCh *m){
    status r = READY;
    SourceFunc func = NULL;
    Abstract *a = NULL;
    Persist *pt = Persist_Get(m);
    PersistItem *item = ((void *)pt)+sizeof(Persist);
    i32 idx = 0;
    while(item != NULL && idx < PERSIST_ARR_MAX){
        MemPage *pg = Span_Get(m->it.p, item->coord.idx);
        a = (Abstract *)(((void *)pg)+item->coord.offset);
        if(a->type.of != TYPE_BLANKED && 
                (func = (SourceFunc)Lookup_Get(BlankerLookup, a->type.of)) != NULL){
            r |= func(sm->m, a, (Abstract *)pt);
        }
        a->type.of = TYPE_BLANKED;
        item++;
        idx++;
    }

    func = (SourceFunc)Lookup_Get(BlankerLookup, pt->type.of);
    func(m, (Abstract *)pt, (Abstract *)pt);
    pt->type.of = TYPE_BLANKED;

    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&m->it);
        if(Stream_Bytes(sm, (byte *)pg, MEM_SLAB_SIZE) != MEM_SLAB_SIZE){
            Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
                "Unable to stream entire page for Persist", NULL);
            break;
        }
    }

    MemCh_Free(m);
    return r;
}

status Persist_FromStream(MemCh *m, Stream *sm){
    status r = READY;
    /*
    MemCh *new = MemCh_Make();
    Persist persist;
    DoFunc func = NULL;
    if(Stream_ReadToMem(sm, sizeof(persist), (byte *)&persist) == sizeof(persist)){
        return ERROR;
    }
    i32 i;
    for(i = 0; i < persist.total; i++){
        MemPage *pg = MemBook_GetPage(m); 
        if(Stream_ReadToMem(sm, MEM_SLAB_SIZE, (byte *)pg) != MEM_SLAB_SIZE){
            return ERROR;
        }
        Span_Add(new->it.p, (Abstract *)pg);
    }

    Table *tbl = Persist_GetTable(m); func = (DoFunc)Lookup_Get(BlankerLookup, tbl->type.of);
    func(new, (Abstract *)tbl);

    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Ref *ref = (Ref *)Iter_Get(&it);
        if(ref != NULL){
            MemPage *pg = Span_Get(m->it.p, ref->coord.idx);
            Abstract *a = (Abstract *)(((void *)pg)+ref->coord.offset);
            if(a->type.of == TYPE_BLANKED && 
                    (func = (DoFunc)Lookup_Get(RepointerLookup, a->type.of)) != NULL){
                r |= func(new, a);
            }
        }
    }

    */
    return r;
}

MemCh *Persist_Make(){
    MemCh *m = MemCh_OnPage();
    m->type.of = TYPE_PERSIST_MEMCTX;
    MemPage *pg = Span_Get(m->it.p, 0);
    Persist *pst = ((void *)pg)+sizeof(MemPage);
    pg->remaining = 0;
    pg->type.state |= MEM_PAGE_PERSIST_ARRAY;
    return m;
}
