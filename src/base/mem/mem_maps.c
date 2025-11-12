#include <external.h>
#include <caneka.h>

static Map *Span_Map(MemCh *m){
    word offset = 0;
    i64 SIZE = 6;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*SIZE), TYPE_RANGE_ARRAY);
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
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*SIZE, TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "Span");
    keys[1] = Str_CstrRef(m, "dims");
    keys[2] = Str_CstrRef(m, "root");
    keys[3] = Str_CstrRef(m, "m");
    keys[4] = Str_CstrRef(m, "nvalues");
    keys[5] = Str_CstrRef(m, "max_idx");
    return Map_Make(m, SIZE-1, atts, keys);
}

static Map *MemCh_Map(MemCh *m){
    word offset = 0;
    i64 SIZE = 5;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*SIZE), TYPE_RANGE_ARRAY);
    atts->of = TYPE_MEMCTX;
    atts->range = 0/*SIZE*/;
    offset += sizeof(Type);
    offset += sizeof(i16);
    (atts+1)->of = TYPE_I16;
    (atts+1)->range = offset;
    offset += sizeof(i16);
    (atts+2)->of = TYPE_SPAN;
    (atts+2)->range = offset+sizeof(Type)+sizeof(i32)/*iter span */;
    offset += sizeof(Iter);
    (atts+3)->of = TYPE_ABSTRACT;
    (atts+3)->range = offset;
    offset += sizeof(Abstract *);
    (atts+4)->of = TYPE_ABSTRACT;
    (atts+4)->range = offset;
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*SIZE, TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "MemCh");
    keys[1] = Str_CstrRef(m, "guard");
    keys[2] = Str_CstrRef(m, "it.p");
    keys[3] = Str_CstrRef(m, "owner");
    keys[4] = Str_CstrRef(m, "values");
    return Map_Make(m, SIZE-1, atts, keys);
}

static Map *MemBookStats_Map(MemCh *m){
    MemBookStats st;
    word offset = 0;
    i64 SIZE = 4;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*SIZE+1), TYPE_RANGE_ARRAY);
    atts->of = TYPE_BOOK_STATS;
    atts->range = SIZE;

    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*SIZE, TYPE_POINTER_ARRAY);

    keys[0] = Str_CstrRef(m, "bookIdx");
    (atts+1)->of = TYPE_I32;
    (atts+1)->range = (void *)&st.bookIdx - (void *)&st;

    keys[1] = Str_CstrRef(m, "pageIdx");
    (atts+2)->of = TYPE_I32;
    (atts+2)->range = (void *)&st.pageIdx - (void *)&st;

    keys[2] = Str_CstrRef(m, "recycled");
    (atts+3)->of = TYPE_I32;
    (atts+3)->range = (void *)&st.recycled - (void *)&st;

    keys[3] = Str_CstrRef(m, "total");
    (atts+4)->of = TYPE_I32;
    (atts+4)->range = (void *)&st.total - (void *)&st;

    return Map_Make(m, SIZE-1, atts, keys);
}

status Mem_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Map(m));
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCh_Map(m));
    r |= Lookup_Add(m, lk, TYPE_BOOK_STATS, (void *)MemBookStats_Map(m));
    return r;
}
