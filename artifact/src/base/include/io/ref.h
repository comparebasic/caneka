#define MEM_PERSIST_MASK 4095 
typedef struct ref {
    Type type;
    Type objType;
    void *ptr;
    struct  {
        quad idx;
        quad offset;
    } coords;
} Ref;

Ref *Ref_Make(MemCh *m);
