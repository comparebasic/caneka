
/*
static Map *Span_Map(MemCh *m){
    Str s;
    RangeType *atts = (RangeType *)Bytes_Alloc(m, sizeof(RangeType)*4);
    atts->of = TYPE_STR;
    atts->range = 4;
    (atts+1)->of = TYPE_WORD;
    (atts+1)->range = (word)((void *)&s.length-(void *)&s);
    (atts+2)->of = TYPE_WORD;
    (atts+2)->range = (word)((void *)&s.alloc-(void *)&s);
    (atts+3)->of = TYPE_BYTES_POINTER;
    (atts+3)->range = (word)((void *)&s.bytes-(void *)&s);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*4);
    keys[0] = Str_CstrRef(m, "Str");
    keys[1] = Str_CstrRef(m, "length");
    keys[2] = Str_CstrRef(m, "alloc");
    keys[3] = Str_CstrRef(m, "bytes");
    return Map_Make(m, 3, atts, keys);
}

status Mem_MapsInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Map(m));
    return r;
}
*/
