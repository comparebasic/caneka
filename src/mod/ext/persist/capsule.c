#include <external.h>
#include <caneka.h>

Lookup *CapsuleDefLookup = NULL;

Capsule *Capsule_Make(MemCh *m, cls typeOf, Buff *in, Buff *out, void *source){
    Capsule *cap = MemCh_AllocOf(m, sizeof(Capsule), TYPE_CAPSULE); 
    cap->type.of = TYPE_CAPSULE;
    cap->objType.of = typeOf;
    cap->source = source;
    cap->in = in;
    cap->out = out;
    return cap;
}

CapsuleDef *CapsuleDef_Make(MemCh *m, 
        Func open, 
        Func close, 
        Func readTo,
        Func writeTo){
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
