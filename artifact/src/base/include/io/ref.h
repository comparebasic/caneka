#define MEM_PERSIST_MASK 4095 

typedef struct ref_coords {
    cls typeOf;
    i16 idx;
    quad offset;
} RefCoord;

typedef struct ref {
    Type type;
    void *ptr;
    RefCoord coord;
} Ref;

Ref *Ref_Make(MemCh *m);
