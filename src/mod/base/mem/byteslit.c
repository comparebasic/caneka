#include <external.h>
#include "base_module.h"

BytesLit *BytesLit_Make(MemCh *m, i16 alloc, cls typeOf){
    BytesLit *sl = MemCh_AllocOf(m, sizeof(RangeType)+alloc, TYPE_BYTE);
    sl->type.of = typeOf;
    sl->type.range = alloc;
    return sl;
}

byte *Bytes_AllocOnPage(MemPage *pg, word sz, cls typeOf){
    word alloc = sz;
    sz += sizeof(RangeType);
    BytesLit *sl = (BytesLit *)MemPage_Alloc(pg, sz);
    sl->type.of = typeOf; 
    sl->type.range = alloc;
    return (byte *)((void *)sl+sizeof(RangeType));
}

byte *Bytes_Alloc(MemCh *m, word alloc, cls typeOf){
    byte *ptr = (byte *)BytesLit_Make(m, alloc, typeOf);
    return ptr+sizeof(RangeType);
}
