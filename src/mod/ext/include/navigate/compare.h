typedef struct compare_result {
    Type type;
    Abstract *a;
    Abstract *b;
} CompResult;

typedef struct comparator {
    Type type;
    i32 count;
    MemCh *m;
    Iter it;
} Comp;

Comp *Comp_Make(MemCh *m, void *a, void *b);
CompResult *CompResult_Make(MemCh *m, void *a, void *b);
status Compare(Comp *comp);
