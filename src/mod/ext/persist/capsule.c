#include <external.h>
#include <caneka.h>

Lookup *CapsuleDefLookup = NULL;

Capsule *Capsule_Make(MemCh *m, cls typeOf, Buff *plain, void *source){
    Capsule *cap = MemCh_AllocOf(m, sizeof(Capsule), TYPE_CAPSULE); 
    cap->type.of = TYPE_CAPSULE;
    cap->objType.of = typeOf;
    cap->source = source;

    CapsuleDef *def = Lookup_Get(CapsuleDefLookup, typeOf); 
    if(def == NULL){
        void *ar[] = {
            Type_ToStr(m, typeOf),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error capsule type not found: $", ar);
        return NULL;
    }
    def->open(m, cap, cap->source);
    if(cap->type.state & ERROR){
        def->close(m, cap, cap->source);
        return NULL;
    }
    return cap;
}

CapsuleDef *CapsuleDef_Make(MemCh *m, 
        SourceFunc open, 
        SourceFunc close, 
        SourceFunc readTo,
        SourceFunc writeTo){
   CapsuleDef *def = MemCh_AllocOf(m, sizeof(CapsuleDef), TYPE_CAPSULE_DEF); 
   def->type.of = TYPE_CAPSULE_DEF;
   def->open = open;
   def->close = close;
   def->readTo = readTo;
   def->writeTo = writeTo;
   return def;
}

void Capsule_Init(MemCh *m){
    if(CapsuleDefLookup == NULL){
        CapsuleDefLookup = Lookup_Make(m, ZERO);
    }
}
