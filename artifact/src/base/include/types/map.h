extern struct lookup *MapsLookup;

typedef Map {
    RangeType type;
    RangeType *atts;
    Str *keys;
}

Map *Map_Make(MemCh *m, i16 length, Arr *atts, Str *keys);
Table *Map_ToTable(MemCh *m, Map *map);
Map *Map_FromTable(MemCh *m, Table *tbl, typeOf);
