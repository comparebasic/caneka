typedef struct hashed {
    Type type;
    int idx;
    util id;
    Abstract *item;
    Abstract *value;
    struct hashed *next;
} Hashed;

extern Chain *HashChain;
status Hash_Init(MemCtx *m);

typedef util (*HashFunc)(Abstract *a);
util Get_Hash(Abstract *a);
Hashed *Hashed_Make(MemCtx *m, Abstract *a);
boolean Hashed_Equals(Hashed *a, Hashed *b);
boolean Hashed_LocalEquals(MemCtx *m, Hashed *a, Hashed *b);
boolean Hashed_ExternalEquals(MemCtx *m, Hashed *a, Hashed *b);
