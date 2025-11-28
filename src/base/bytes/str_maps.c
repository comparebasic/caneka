#include <external.h>
#include <caneka.h>
#include "../module.h"

static Map *Str_Map(MemCh *m){
    Str s;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, sizeof(RangeType)*4, TYPE_RANGE_ARRAY);
    atts->of = TYPE_STR;
    atts->range = 4;
    (atts+1)->of = TYPE_WORD;
    (atts+1)->range = (word)((void *)&s.length-(void *)&s);
    (atts+2)->of = TYPE_WORD;
    (atts+2)->range = (word)((void *)&s.alloc-(void *)&s);
    (atts+3)->of = TYPE_BYTES_POINTER;
    (atts+3)->range = (word)((void *)&s.bytes-(void *)&s);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*4, TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "Str");
    keys[1] = Str_CstrRef(m, "length");
    keys[2] = Str_CstrRef(m, "alloc");
    keys[3] = Str_CstrRef(m, "bytes");
    return Map_Make(m, 3, atts, keys);
}

static Map *StrVec_Map(MemCh *m){
    StrVec v;
    word offset = 0;
    i16 size = 2;
    RangeType *atts = (RangeType *)Bytes_Alloc(m,
        sizeof(RangeType)*(size+1), TYPE_RANGE_ARRAY);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*(size+1), TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "StrVec");
    atts->of = TYPE_STRVEC;
    atts->range = size+1;
    keys[1] = Str_CstrRef(m, "total");
    (atts+1)->of = TYPE_I64;
    (atts+1)->range = (word)((void *)&v.total-(void *)&v);
    (atts+2)->of = TYPE_SPAN;
    keys[2] = Str_CstrRef(m, "p");
    (atts+2)->range = (word)((void *)&v.p-(void *)&v);;
    return Map_Make(m, size, atts, keys);
}

status Str_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Map(m));
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Map(m));
    return r;
}
