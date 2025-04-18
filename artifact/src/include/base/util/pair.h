typedef struct pair {
    Type type;
    Abstract *a;
    Abstract *b;
} Pair;

Pair *Pair_Make(MemCh *m);
