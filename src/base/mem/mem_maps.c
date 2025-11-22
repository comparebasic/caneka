#include <external.h>
#include <caneka.h>

static Map *Span_Map(MemCh *m){
    Span p;
    i32 size = 6;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*(size+1)), TYPE_RANGE_ARRAY);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*(size+1), TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "Span");
    atts->of = TYPE_SPAN;
    atts->range = size+1;

    keys[1] = Str_CstrRef(m, "dims");
    (atts+1)->of = TYPE_I8;
    (atts+1)->range = (word)((void *)&p.dims - (void *)&p);
    keys[2] = Str_CstrRef(m, "memLevel");
    (atts+2)->of = TYPE_I16;
    (atts+2)->range =  (word)((void *)&p.dims - (void *)&p);
    keys[3] = Str_CstrRef(m, "m");
    (atts+3)->of = TYPE_MEMCTX;
    (atts+3)->range = (word)((void *)&p.m - (void *)&p);
    keys[4] = Str_CstrRef(m, "root");
    (atts+4)->of = TYPE_POINTER_ARRAY;
    (atts+4)->range = (word)((void *)&p.root - (void *)&p);
    keys[5] = Str_CstrRef(m, "nvalues");
    (atts+5)->of = TYPE_I32;
    (atts+5)->range = (word)((void *)&p.nvalues - (void *)&p);
    keys[6] = Str_CstrRef(m, "max_idx");
    (atts+6)->of = TYPE_I32;
    (atts+6)->range = (word)((void *)&p.nvalues - (void *)&p);
    return Map_Make(m, size-1, atts, keys);
}

static Map *MemCh_Map(MemCh *m){
    MemCh _m;
    word offset = 0;
    i32 size = 5;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*(size+1)), TYPE_RANGE_ARRAY);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*(size+1), TYPE_POINTER_ARRAY);
    keys[0] = Str_CstrRef(m, "MemCh");
    atts->of = TYPE_MEMCTX;
    atts->range = size+1;
    keys[1] = Str_CstrRef(m, "guard");
    (atts+1)->of = TYPE_I16;
    (atts+1)->range = (word)((void *)&_m.guard - (void *)&_m);
    keys[2] = Str_CstrRef(m, "itp");
    (atts+2)->of = TYPE_SPAN;
    (atts+2)->range = (word)((void *)&_m.it.p - (void *)&_m);
    keys[3] = Str_CstrRef(m, "owner");
    (atts+3)->of = TYPE_ABSTRACT;
    (atts+3)->range = (word)((void *)&_m.owner - (void *)&_m);
    return Map_Make(m, size-1, atts, keys);
}

static Map *MemBookStats_Map(MemCh *m){
    MemBookStats st;
    word offset = 0;
    i64 SIZE = 4;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, 
        (word)(sizeof(RangeType)*SIZE+1), TYPE_RANGE_ARRAY);
    atts->of = TYPE_BOOK_STATS;
    atts->range = SIZE;

    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*SIZE+1, TYPE_POINTER_ARRAY);

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

    return Map_Make(m, SIZE, atts, keys);
}

status Mem_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Map(m));
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCh_Map(m));
    r |= Lookup_Add(m, lk, TYPE_BOOK_STATS, (void *)MemBookStats_Map(m));
    return r;
}
