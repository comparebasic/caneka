#include <external.h>
#include <caneka.h>

static Map *Span_Map(MemCh *m){
    word offset = 0;
    #define SIZE 6
    RangeType *atts = (RangeType *)Bytes_Alloc(m, sizeof(RangeType)*SIZE, TYPE_RANGE_ARRAY);
    atts->of = TYPE_SPAN;
    atts->range = SIZE;
    offset += sizeof(Type);
    offset += sizeof(word);
    offset += sizeof(byte);
    (atts+1)->of = TYPE_I8;
    (atts+1)->range = offset;
    offset += sizeof(i8);
    (atts+2)->of = TYPE_POINTER_ARRAY;
    (atts+2)->range = offset;
    offset += sizeof(slab *);
    (atts+3)->of = TYPE_MEMCTX;
    (atts+3)->range = offset;
    offset += sizeof(MemCh *);
    (atts+4)->of = TYPE_I32;
    (atts+4)->range = offset;
    offset += sizeof(i32);
    (atts+5)->of = TYPE_I32;
    (atts+5)->range = offset;
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*4, TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "Span");
    keys[1] = Str_CstrRef(m, "dims");
    keys[2] = Str_CstrRef(m, "root");
    keys[3] = Str_CstrRef(m, "m");
    keys[4] = Str_CstrRef(m, "nvalues");
    keys[5] = Str_CstrRef(m, "max_idx");
    return Map_Make(m, SIZE-1, atts, keys);
}

status Mem_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Map(m));
    return r;
}
