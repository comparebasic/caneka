#include <external.h>
#include <caneka.h>

StrLit *StrLit_Make(MemCh *m, i16 alloc, cls typeOf){
    StrLit *sl = MemCh_AllocOf(m, sizeof(RangeType)+alloc, TYPE_BYTE);
    sl->type.of = typeOf;
    sl->type.range = alloc;
    return sl;
}

byte *Bytes_AllocOnPage(MemPage *pg, word sz, cls typeOf){
    word alloc = sz;
    sz += sizeof(RangeType);
    StrLit *sl = (StrLit *)MemPage_Alloc(pg, sz);
    sl->type.of = typeOf; 
    sl->type.range = alloc;
    return (byte *)((void *)sl+sizeof(RangeType));
}

byte *Bytes_Alloc(MemCh *m, word alloc, cls typeOf){
    byte *ptr = (byte *)StrLit_Make(m, alloc, typeOf);
    return ptr+sizeof(RangeType);
}
