#include <external.h>
#include <caneka.h>

byte *StrLit_BytesMake(MemCh *m, word alloc){
    StrLit *sl = StrLit_Make(m, alloc);
    return (byte *)((void *)sl+sizeof(RangeType));
}

byte *StrLit_PageBytesMake(MemPage *pg, word sz){
    word alloc = sz;
    sz += sizeof(RangeType);
    StrLit *sl = (StrLit *)MemPage_Alloc(pg, sz);
    sl->type.of = TYPE_STRLIT; 
    sl->type.range = alloc;
    return (byte *)((void *)sl+sizeof(RangeType));
}

StrLit *StrLit_Make(MemCh *m, i16 alloc){
    StrLit *sl = MemCh_Alloc(m, sizeof(RangeType)+alloc);
    sl->type.of = TYPE_STRLIT; 
    sl->type.range = alloc;
    return  sl;
}
