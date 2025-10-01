#include <external.h>
#include <caneka.h>

static Map *Table_Map(MemCh *m){
    return Lookup_Get(MapsLookup, TYPE_SPAN);
}

static Map *Hashed_Map(MemCh *m){
    Hashed h;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, sizeof(RangeType)*6, TYPE_RANGE_ARRAY);
    atts->of = TYPE_HASHED;
    atts->range = 6;
    (atts+1)->of = TYPE_I32;
    (atts+1)->range = (word)((void *)&h.idx-(void *)&h);
    (atts+2)->of = TYPE_I32;
    (atts+2)->range = (word)((void *)&h.orderIdx-(void *)&h);
    (atts+3)->of = TYPE_UTIL;
    (atts+3)->range = (word)((void *)&h.id-(void *)&h);
    (atts+4)->of = TYPE_ABSTRACT;
    (atts+4)->range = (word)((void *)&h.key-(void *)&h);
    (atts+5)->of = TYPE_ABSTRACT;
    (atts+5)->range = (word)((void *)&h.value-(void *)&h);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*4, TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "Hashed");
    keys[1] = Str_CstrRef(m, "idx");
    keys[2] = Str_CstrRef(m, "orderIdx");
    keys[3] = Str_CstrRef(m, "id");
    keys[4] = Str_CstrRef(m, "key");
    keys[5] = Str_CstrRef(m, "value");
    return Map_Make(m, 5, atts, keys);
}

status Sequence_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Table_Map(m));
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Map(m));
    return r;
}
