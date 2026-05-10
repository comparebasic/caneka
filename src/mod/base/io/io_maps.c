#include <external.h>
#include "base_module.h"

static Map *Buff_Map(MemCh *m){
    Buff bf;
    i32 size = 4;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*(size+1)), TYPE_RANGE_ARRAY);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*(size+1), TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "Buff");
    atts->of = TYPE_SPAN;
    atts->range = size+1;

    keys[1] = Str_CstrRef(m, "m");
    (atts+1)->of = TYPE_MEMCTX;
    (atts+1)->range = (word)((void *)&bf.m - (void *)&bf);

    keys[2] = Str_CstrRef(m, "fd");
    (atts+2)->of = TYPE_I32;
    (atts+2)->range = (word)((void *)&bf.fd - (void *)&bf);

    keys[3] = Str_CstrRef(m, "v");
    (atts+3)->of = TYPE_STRVEC;
    (atts+3)->range =  (word)((void *)&bf.v - (void *)&bf);

    return Map_Make(m, size-1, atts, keys);
}

status Io_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_BUFF, (void *)Buff_Map(m));
    return r;
}
