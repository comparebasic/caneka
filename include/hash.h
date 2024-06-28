typedef struct hashed {
    Type type;
    int idx;
    util id;
    Abstract *item;
} Hashed;

typedef struct hashed_linked {
    Type type;
    int idx;
    util id;
    Abstract *item;
    struct hashed_linked *next;
} HashedLinked;


extern Chain *HashChain;
status Hash_Init(MemCtx *m);

typedef util (*HashFunc)(Abstract *a);
util Get_Hash(Abstract *a);
Hashed *Hashed_Make(MemCtx *m, Abstract *a);
