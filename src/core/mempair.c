#include <external.h>
#include <caneka.h>

static Source *memPair_LoadSlab(MemPersist *mp, word idx){
    Source *slab_s = Span_Get(mp->map, idx);
    if(slab_s != NULL){
        return slab_s->bytes;
    }
    Source *file = Source_Read(map, mp->path, String_FromNum(mp->m, idx));
    if((file->type.state & SUCCESS)){
        return Span_Set(mp->map, idx, file->bytes)->bytes;
    }
    return NULL;
}

static void *memPair_FetchAddr(MemPersist *mp, void *addrPtr){
    MemPair pair;
    memset(&pair, 0, sizeof(MemPair));
    pair.type.of = TYPE_MEMPAIR;
    pair.slab = MemCtx_GetSlab(pair->m, addrPtr);
    if(pair.slab == NULL){
        return NULL;
    }
    pair.offset = addrPtr - pair.slab;

    Source *MemPair_LoadSlab(pair.slab->idx);
    return source + pair.offset;
}

void *MemPair_GetAddr(MemPersist *mp, void *addr){
    if(addr == NULL){
        return memPair_FetchAddr(mp, &addr);
    }
    return addr;
}
