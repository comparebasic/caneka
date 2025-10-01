#include <external.h>
#include <caneka.h>

static boolean _mapsInitialized = FALSE;
struct lookup *MapsLookup = NULL;

Map *Map_Make(MemCh *m, i16 length, RangeType *atts, Str **keys){
    Map *map = MemCh_AllocOf(m, sizeof(Map), TYPE_MAP);
    map->type.of = TYPE_MAP;
    map->type.range = length;
    map->atts = atts;
    map->keys = keys;
    return map;
}

Table *Map_ToTable(MemCh *m, Map *map){
    return NULL;
}

Map *Map_FromTable(MemCh *m, Table *tbl, cls typeOf){
    return NULL;
}

status Maps_Init(MemCh *m){
    status r = READY;
    if(!_mapsInitialized){
        _mapsInitialized = TRUE;
        MapsLookup = Lookup_Make(m, ZERO);
        r |= Str_MapsInit(m, MapsLookup); 
        r |= Sequence_MapsInit(m, MapsLookup); 
        r |= Mem_MapsInit(m, MapsLookup); 
        r |= SUCCESS;
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
