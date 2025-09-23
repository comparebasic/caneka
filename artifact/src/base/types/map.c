#include <external.h>
#include <caneka.h>

Map *Map_Make(MemCh *m, i16 length, Arr *atts, Str *keys){
    Map *map = MemCh_AllocOf(m, sizeof(Map), TYPE_MAP);
    map->type.of = TYPE_MAP;
    map->type.range = length;
    map->atts = atts;
    map->keys = keys;
    return map;
}

Table *Map_ToTable(MemCh *m, Map *map);
Map *Map_FromTable(MemCh *m, Table *tbl, cls typeOf);
