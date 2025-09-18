#include <external.h>
#include <caneka.h>

struct lookup *BlankerLookup = NULL;

Table *Persist_GetTable(MemCh *m){
    if(m->type.of != TYPE_PERSIST_MEMCTX){
        Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
            "Mismatch of type when trying to get the span of a persist memchapter.", NULL);
        return NULL;
    }
    void *pg = Span_Get(m->it.p, 0);
    i64 offset = sizeof(MemCh)+sizeof(Table)+sizeof(slab);
    return (Table *)as((pg+MEM_SLAB_SIZE)-offset-sizeof(Table), TYPE_SPAN);
}

status Persist_SetRef(MemCh *m, i32 slIdx, MemPage *pg, void *ptr){
    Ref *ref = Ref_Make(m);
    ref->coords.idx = slIdx;
    ref->coords.offset = (quad)(((util)ptr) & MEM_PERSIST_MASK);
    ref->ptr = ptr;

    i32 idx =  Table_Set(Persist_GetTable(m),
        (Abstract *)Util_Wrapped(m, (util)ptr),
        (Abstract *)ref);
    return idx >= 0 ?  SUCCESS : ERROR;
}

MemCh *Persist_Make(){
    MemCh *m = MemCh_OnPage();
    Table_Make(m);
    m->type.of = TYPE_PERSIST_MEMCTX;
    return m;
}

status Persist_FlushFree(Stream *sm, MemCh *m, StrVec *path){
    Iter it;
    Table *p = Persist_GetTable(m);
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Ref *ref = (Ref *)Iter_Get(&it);
        MemPage *pg = Span_Get(m->it.p, ref->coords.idx);
        Abstract *a = (Abstract *)(((void *)pg)+ref->coords.offset);
        ref->objType.of = a->type.of;
        SourceFunc func = NULL;
        if(a->type.of != TYPE_BLANKED && 
                (func = (SourceFunc)Lookup_Get(BlankerLookup, a->type.of)) != NULL){
            func(m, a, (Abstract *)p);
        }
        a->type.of = TYPE_BLANKED;
    }

    Iter_Reset(&m->it);
    Persist persist = {
        .type = {.of = TYPE_PERSIST, .state = ZERO},
        .total = m->it.p->nvalues
    };
    Stream_Bytes(sm, (byte *)&persist, sizeof(Persist));
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)Iter_Get(&it);
        if(Stream_Bytes(sm, (byte *)pg, MEM_SLAB_SIZE) != MEM_SLAB_SIZE){
            Error(ErrStream->m, (Abstract *)m, FUNCNAME, FILENAME, LINENUMBER,
                "Unable to stream entire page for Persist", NULL);
            break;
        }
    }

    MemCh_Free(m);
    return SUCCESS;
}
