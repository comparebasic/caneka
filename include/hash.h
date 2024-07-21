typedef struct hashed {
    Type type;
    util id;
    Abstract *item;
    Abstract *value;
    int idx;
    struct hashed *next;
} Hashed;

extern Chain *HashChain;
status Hash_Init(MemCtx *m);

typedef util (*HashFunc)(Abstract *a);
util Get_Hash(Abstract *a);
Hashed *Hashed_Make(MemCtx *m, Abstract *a);
