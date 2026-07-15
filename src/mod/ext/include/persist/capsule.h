typedef struct capsule {
    Type type;
    Type objType;
    MemCh *m;
    Buff *in;
    Buff *out;
    void *source;
} Capsule;

typedef struct capsule_def {
    Type type;
    Func open;
    Func close;
    Func readTo;
    Func writeTo;
} CapsuleDef;

extern Lookup *CapsuleDefLookup;

Capsule *Capsule_Make(MemCh *m, cls typeOf, Buff *in, Buff *out, void *source);

CapsuleDef *CapsuleDef_Make(MemCh *m, 
    Func open, 
    Func close, 
    Func readTo,
    Func writeTo);

void Capsule_Init(MemCh *m);
