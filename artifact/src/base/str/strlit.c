#include <external.h>
#include <caneka.h>

StrLit *StrLit_Make(MemCh *m, i16 alloc){
    StrLit *sl = MemCh_AllocOf(m, sizeof(RangeType)+alloc, TYPE_BYTE);
    sl->type.of = TYPE_BYTES_POINTER;
    sl->type.range = alloc;
    return  sl;
}

byte *Bytes_AllocOnPage(MemPage *pg, word sz){
    word alloc = sz;
    sz += sizeof(RangeType);
    StrLit *sl = (StrLit *)MemPage_Alloc(pg, sz);
    sl->type.of = TYPE_BYTES_POINTER; 
    sl->type.range = alloc;
    return (byte *)((void *)sl+sizeof(RangeType));
}

byte *Bytes_Alloc(MemCh *m, word alloc){
    StrLit *sl = StrLit_Make(m, alloc);
    return (byte *)((void *)sl+sizeof(RangeType));
}
