typedef struct pair {
    Type type;
    void *a;
    void *b;
} Pair;

Pair *Pair_Make(MemCh *m);
