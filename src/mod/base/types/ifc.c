#include <external.h>
#include "base_module.h"

Lookup *IfcLookup = NULL;

void Type_SetFlag(void *_a, word flags){
    Abstract *a = (Abstract *)_a;
    a->type.state = (a->type.state & NORMAL_FLAGS) | flags;
}

void *Ifc(MemCh *m, void *_a, cls typeOf){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == typeOf){
        return a;
    }

    void *map = Lookup_Get(IfcLookup, a->type.of);
    if(map != NULL){
        if(map->type.of == TYPE_SPAN){
            Iter it;
            Iter_Init(&it, (Span *)map);
            while((Iter_Next(&it) & END) == 0){
                IfcMap *imap = Iter_Get(&it);
                if(typeOf <= (imap->objType.of + imap->offset->end) &&
                            typeOf >= (imap->objType.of + imap->offset->start)){

                        if(imap->func != NULL){
                            void *o = func(m, a, imap);
                            if(o != NULL){
                                return o;
                            }
                        }else{
                            return a;
                        }
                    }
                }
            }
        }else if(map->type.of == TYPE_IFC_MAP){
            IfcMap *imap = (IfcMap *)map;
            if(typeOf <= (imap->objType.of + imap->offset->end) &&
                        typeOf >= (imap->objType.of + imap->offset->start)){

                    if(imap->func != NULL){
                        void *o = func(m, a, imap);
                        if(o != NULL){
                            return o;
                        }
                    }else{
                        return a;
                    }
                }
            }
        }
    }

    void *ar[] = {Type_ToStr(m, a->type.of), imap, NULL};
    Error(m, FUNCNAME, FILENAME, LINENUMBER,
        "Ifc mismatch no imap found to coorce type @, found @", ar);
    return NULL;
}

IfcMap *IfcMap_Make(MemCh *m,
        cls typeOf, i16 start, i16 end, i64 size, SourceMakerFunc func){
    IfcMap *imap = MemCh_AllocOf(m, sizeof(IfcMap) TYPE_IFC_MAP);
    imap->type.of = TYPE_IFC_MAP;
    imap->objType.of = typeOf;
    imap->offset.start = start;
    imap->offset.end = end;
    imap->size = size;
    imap->func = func;
    return imap;
}

void Ifc_Init(MemCh *m){
    if(IfcLookup == NULL){
        IfcLookup = Lookup_Make(m, ZERO);
        ConvLookup = Lookup_Make(m, ZERO);
    }
}
