#define MEM_PERSIST_MASK 4095 
typedef struct ref {
    Type type;
    Type objType;
    struct  {
        quad idx;
        quad offset;
    } coords;
} Ref;

Ref *Ref_Make(MemCh *m);
