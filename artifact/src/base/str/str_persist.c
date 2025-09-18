#include <external.h>
#include <caneka.h>

static status Str_Blanker(MemCh *pm, Abstract *a, Abstract *_tbl){
    Table *tbl = _tbl;
    Str *s = (Str *)a;
    Ref *ref = NULL; 
    if(Persist_FillRef(tbl, s->bytes, &ref) & SUCCESS){
        memcpy((void *)&s->bytes, &ref->coords, sizeof(void *));
        return SUCCESS;
    }
    return NOOP;
}

static status Str_Repointer(MemCh *pm, Abstract *a){
    Table *tbl = _tbl;
    Str *s = (Str *)a;
    s->type.of = Persist_RepointAddr(pm, &s->bytes);
    return SUCCESS;
}

/* 
   strvec
   span
   single
   table
   hashed
*/

status Str_PersistInit(MemCh *m){
    status r = READY;
    r |= Lookup_Add(BlankerLookup, TYPE_STR, (void *)Str_Blanker);

    r |= Lookup_Add(RepointerLookup, TYPE_STR, (void *)Str_Repointer);
    return r;
}
