#include <external.h>
#include <caneka.h>

static Map *Str_Map(MemCh *m){
    Str s;
    RangeType *atts = Bytes_Alloc(m, sizeof(RangeType)*4);
    (RangeType *(atts))->type.of = TYPE_STR;
    (RangeType *(atts+1))->type.of = TYPE_WORD;
    (RangeType *(atts+1))->type.range = (word)(s.length-s);
    (RangeType *(atts+2))->type.of = TYPE_WORD;
    (RangeType *(atts+2))->type.range = (word)(s.alloc-s);
    (RangeType *(atts+3))->type.of = TYPE_BYTES_POINTER;
    (RangeType *(atts+3))->type.range = (word)(s.bytes-s);
    Str **keys = Bytes_Alloc(m, sizeof(Str *)*4);
    keys[0] = Str_CstrRef(m, "Str");
    keys[1] = Str_CstrRef(m, "length");
    keys[2] = Str_CstrRef(m, "alloc");
    keys[3] = Str_CstrRef(m, "bytes");
    return Map_Make(m, 3, atts, keys);
}

status Str_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Map);
    return r;
}
