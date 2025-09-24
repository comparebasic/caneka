extern struct lookup *MapsLookup;

typedef struct map {
    RangeType type;
    RangeType *atts;
    Str **keys;
} Map;

Map *Map_Make(MemCh *m, i16 length, RangeType *atts, Str **keys);
Table *Map_ToTable(MemCh *m, Map *map);
Map *Map_FromTable(MemCh *m, Table *tbl, cls typeOf);
status Maps_Init(MemCh *m);
