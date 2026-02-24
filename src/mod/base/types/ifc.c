#include <external.h>
#include "base_module.h"

Lookup *IfcLookup = NULL;

void Type_SetFlag(void *_a, word flags){
    Abstract *a = (Abstract *)_a;
    a->type.state = (a->type.state & NORMAL_FLAGS) | flags;
}

IfcMap *Ifc_Get(Abstract *a, cls typeOf){
    IfcMap *imap = Lookup_Get(IfcLookup, a->type.of);
    if(imap != NULL && (typeOf <= (imap->typeOf + imap->offset.end) &&
            typeOf >= (imap->typeOf + imap->offset.start))){
        return imap;
    }
    return NULL;
}

cls Ifc_GetRoot(cls typeOf){
    if(typeOf == TYPE_SPAN || typeOf == TYPE_TABLE){
        return TYPE_SPAN;
    }else if(typeOf > _TYPE_WRAPPED_START && typeOf < _TYPE_WRAPPED_END){
        return TYPE_WRAPPED;
    }
    return typeOf;
}

void *Ifc(MemCh *m, void *_a, cls typeOf){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == typeOf){
        return a;
    }

    IfcMap *imap = Ifc_Get(a, typeOf);
    if(imap != NULL){
        if(imap->func != NULL){
            void *o = imap->func(m, a, imap);
            if(o != NULL){
                return o;
            }
        }else{
            return a;
        }
    }

    void *ar[] = {Type_ToStr(m, a->type.of), imap, NULL};
    Error(m, FUNCNAME, FILENAME, LINENUMBER,
        "Ifc mismatch no imap found to coorce type @, found @", ar);
    return NULL;
}

IfcMap *IfcMap_Make(MemCh *m,
        cls typeOf, i16 start, i16 end, i16 size, SourceMakerFunc func){
    IfcMap *imap = MemCh_AllocOf(m, sizeof(IfcMap), TYPE_IFC_MAP);
    imap->type.of = TYPE_IFC_MAP;
    imap->typeOf = typeOf;
    imap->offset.start = start;
    imap->offset.end = end;
    imap->size = size;
    imap->func = func;
    return imap;
}

void Ifc_Init(MemCh *m){
    if(IfcLookup == NULL){
        IfcLookup = Lookup_Make(m, ZERO);
    }
}
