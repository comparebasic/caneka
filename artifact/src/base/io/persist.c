#include <external.h>
#include <caneka.h>

static boolean _initialized = FALSE;
struct lookup *BlankerLookup = NULL;
struct lookup *RepointerLookup = NULL;

status Persist_FillRef(Table *tbl, void *ptr, Ref **ref){
    Single sg = {
        .type = {.of = TYPE_WRAPPED_UTIL, .state = ZERO},
        .objType = {.of = ZERO, .state = ZERO},
        .val.value = (util)ptr
    };
    *ref = (Ref *)Table_Get(tbl, (Abstract *)&sg);
    if(*ref != NULL){
        return SUCCESS;
    }
    return NOOP;
}

cls Persist_RepointAddr(MemCh *pm, void **ptr){
    RefCoord *coord = (RefCoord *)ptr;
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

Table *Persist_GetTable(MemCh *m){
    if(m->type.of != TYPE_PERSIST_MEMCTX){
        Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
            "Mismatch of type when trying to get the span of a persist memchapter.", NULL);
        return NULL;
    }
    MemPage *pg = Span_Get(m->it.p, 0);
    void **location = pg+sizeof(MemPage);
    return (Table *)as(*location, TYPE_TABLE);
}

status Persist_SetRef(MemCh *m, i32 slIdx, MemPage *pg, void *ptr){
    Ref *ref = Ref_Make(m);
    ref->coord.idx = slIdx;
    ref->coord.offset = (quad)(((util)ptr) & MEM_PERSIST_MASK);
    ref->ptr = ptr;

    i32 idx =  Table_Set(Persist_GetTable(m),
        (Abstract *)Util_Wrapped(m, (util)ptr),
        (Abstract *)ref);
    return idx >= 0 ?  SUCCESS : ERROR;
}

status Persist_FlushFree(Stream *sm, MemCh *m){
    status r = READY;
    Iter it;
    Table *tbl = Persist_GetTable(m);
    Iter_Init(&it, tbl);
    SourceFunc func = NULL;
    Abstract *a = NULL;
    while((Iter_Next(&it) & END) == 0){
        Ref *ref = (Ref *)Iter_Get(&it);
        if(ref != NULL){
            MemPage *pg = Span_Get(m->it.p, ref->coord.idx);
            a = (Abstract *)(((void *)pg)+ref->coord.offset);
            if(a->type.of != TYPE_BLANKED && 
                    (func = (SourceFunc)Lookup_Get(BlankerLookup, a->type.of)) != NULL){
                r |= func(sm->m, a, (Abstract *)tbl);
            }
            a->type.of = TYPE_BLANKED;
        }
    }

    a = (Abstract *)tbl;
    func = (SourceFunc)Lookup_Get(BlankerLookup, a->type.of);
    func(m, a, a);
    a->type.of = TYPE_BLANKED;

    Persist persist = {
        .type = {.of = TYPE_PERSIST, .state = ZERO},
        .total = m->it.p->nvalues
    };
    Stream_Bytes(sm, (byte *)&persist, sizeof(Persist));

    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&it);
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

    return SUCCESS;
}

MemCh *Persist_Make(){
    MemCh *m = MemCh_OnPage();
    MemPage *pg = Span_Get(m->it.p, 0);
    Table *tbl = Table_Make(m);
    pg->remaining -= sizeof(void *);
    void **location = pg+sizeof(MemPage);
    *location = (void *)tbl;
    m->type.of = TYPE_PERSIST_MEMCTX;
    return m;
}

