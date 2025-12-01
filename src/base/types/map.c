#include <external.h>
#include "base_module.h"

static boolean _mapsInitialized = FALSE;
struct lookup *MapsLookup = NULL;

void *Map_FromOffset(MemCh *m, void *a, i16 offset, cls typeOf){
    void *value = NULL;
    if(typeOf == ZERO || typeOf > _TYPE_RAW_END){
        void **ptr = (void **)(((void *)a)+offset);
        value = *ptr; 
    }else{
        if(typeOf == TYPE_UTIL){
            util *ptr = ((void *)a)+offset;
            value = Util_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I32){
            i32 *ptr = ((void *)a)+offset;
            value = I32_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I64){
            i64 *ptr = ((void *)a)+offset;
            value = I64_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I16){
            i16 *ptr = ((void *)a)+offset;
            value = I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_I8){
            i8 *ptr = ((void *)a)+offset;
            value = I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_BYTE){
            byte *ptr = ((void *)a)+offset;
            value = B_Wrapped(m, *ptr, ZERO, ZERO); 
        }
    }
    return value;
}

Map *Map_Make(MemCh *m, i16 length, RangeType *atts, Str **keys){
    Map *map = MemCh_AllocOf(m, sizeof(Map), TYPE_MAP);
    map->type.of = TYPE_MAP;
    map->type.range = length;
    map->atts = atts;
    map->keys = keys;
    Map_MakeTbl(m, map);
    return map;
}

status Map_MakeTbl(MemCh *m, Map *map){
    map->tbl = Table_Make(m); 
    for(i16 i = 0; i < map->type.range; i++){
        RangeType *att = map->atts+(i+1);
        Str *s = map->keys[i];
        Table_Set(map->tbl, s, att);
    }
    return SUCCESS;
}

Map *Map_Get(cls typeOf){
    return Lookup_Get(MapsLookup, typeOf);
}

Table *Map_ToTable(MemCh *m, void *_a){
    Abstract *a = (Abstract *)_a;
    Map *map = Lookup_Get(MapsLookup, a->type.of);
    if(map == NULL){
        return NULL;
    }

    Table *tbl = Table_Make(m);

    Iter it;
    Iter_Init(&it, map->tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            RangeType *def = h->value;
            
            Abstract *value = Map_FromOffset(m, a, def->range, def->of);
            if(value != NULL){
                Table_Set(tbl, h->key, value);
            }
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
        r |= Mem_MapsInit(m, MapsLookup); 
        r |= Sequence_MapsInit(m, MapsLookup); 
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
