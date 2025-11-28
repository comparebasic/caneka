#include <external.h>
#include <caneka.h>
#include "../module.h"

static Map *Hashed_Map(MemCh *m){
    Hashed h;
    i16 size = 5;
    RangeType *atts = (RangeType *)Bytes_Alloc(m,
        sizeof(RangeType)*(size+1), TYPE_RANGE_ARRAY);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*(size+1), TYPE_POINTER_ARRAY);
    atts->of = TYPE_HASHED;
    atts->range = size+1;
    keys[0] = Str_CstrRef(m, "idx");
    (atts+1)->of = TYPE_I32;
    (atts+1)->range = (word)((void *)&h.idx-(void *)&h);
    keys[1] = Str_CstrRef(m, "orderIdx");
    (atts+2)->of = TYPE_I32;
    (atts+2)->range = (word)((void *)&h.orderIdx-(void *)&h);
    keys[2] = Str_CstrRef(m, "id");
    (atts+3)->of = TYPE_UTIL;
    (atts+3)->range = (word)((void *)&h.id-(void *)&h);
    keys[3] = Str_CstrRef(m, "key");
    (atts+4)->of = TYPE_ABSTRACT;
    (atts+4)->range = (word)((void *)&h.key-(void *)&h);
    keys[4] = Str_CstrRef(m, "value");
    (atts+5)->of = TYPE_ABSTRACT;
    (atts+5)->range = (word)((void *)&h.value-(void *)&h);
    return Map_Make(m, size, atts, keys);
}

status Sequence_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Lookup_Get(MapsLookup, TYPE_SPAN));
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Map(m));
    return r;
}
