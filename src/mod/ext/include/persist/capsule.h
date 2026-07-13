typedef struct capsule {
    Type type;
    Type objType;
    MemCh *m;
    Buff *plain;
    void *source;
} Capsule;

typedef struct capsule_def {
    Type type;
    SourceFunc open, 
    SourceFunc close, 
    SourceFunc readTo,
    SourceFunc writeTo;
} CapsuleDef;

extern Lookup *CapsuleDefLookup;

Capsule *Capsule_Make(MemCh *m, Buff *enc, Buff *plain, CapsuleDef *def);

CapsuleDef *CapsuleDef_Make(MemCh *m, 
    SourceFunc open, 
    SourceFunc close, 
    SourceFunc readTo,
    SourceFunc writeTo);


void Capsule_Init(MemCh *m);
