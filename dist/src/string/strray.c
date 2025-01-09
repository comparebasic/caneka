#include <external.h>
#include <caneka.h>
/*
static struct coords {
    int slabIdx;
    int offset;
};

static void getCoords(Strray *srr, struct coords *crd, i64 idx){
    crd->slabIdx = idx / srr->stride;
    crd->offset = idx % srr->stride;
}

Strray *Strray_Make(MemCtx *m size_t sz){
   Sttray *srr = (Strray *)MemCtx_Alloc(m, sizeof(Strray)); 
   srr->type.of = TYPE_STRRAY;
   srr->sz = sz;
   srr->stride = STRING_CHUNK_SIZE / sz;
   srr->indicies = Span_Make(m, TYPE_SPAN);
   return srr;
}

status Strray_Get(Strray *srr, MemCtx *m, void **v){
    struct coords crd;
    getCoords(srr, &crd, idx);
    StrraySlab *sl = Span_Get(srr->indicies, crd.slabIdx);
    if(sl == NULL){
        return MISS;
    }else{
        *v = sl->bytes+crd.offset;
        return SUCCESS;
    }
}
*/
