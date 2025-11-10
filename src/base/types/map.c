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

Table *Map_ToTable(MemCh *m, void *_a){
    Abstract *a = (Abstract *)_a;
    Map *map = Lookup_Get(MapsLookup, a->type.of);
    if(map == NULL){
        return NULL;
    }

    Table *tbl = Table_Make(m);
    for(i16 i = 0; i < map->type.range; i++){
        Str *key = map->keys[i];
        Abstract *value = NULL;
        RangeType *def = map->atts+i;
        void *att = a+def->range;
        if(def->of == TYPE_I32){
            value = (Abstract *)I32_Wrapped(m, *((i32 *)att));
        }else if(def->of == TYPE_I64){
            value = (Abstract *)I32_Wrapped(m, *((i64 *)att));
        }else if(def->of == TYPE_I16){
            value = (Abstract *)I32_Wrapped(m, *((i16 *)att));
        }else if(def->of == TYPE_I8){
            value = (Abstract *)I32_Wrapped(m, *((i8 *)att));
        }else if(def->of > _TYPE_ABSTRACT_BEGIN){
            value = (Abstract *)att;
        }

        if(value != NULL){
            Table_Set(tbl, key, value);
        }
    }

    return tbl;
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
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
