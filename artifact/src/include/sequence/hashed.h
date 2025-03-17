typedef struct hashed {
    Type type;
    int idx;
    util id;
    Abstract *item;
    Abstract *value;
} Hashed;

extern struct chain *HashChain;
status Hash_Init(MemCtx *m);

typedef util (*HashFunc)(Abstract *a);
util Get_Hash(Abstract *a);
util Hash_Bytes(byte *bt, size_t length);
Hashed *Hashed_Make(MemCtx *m, Abstract *a);
Hashed *Hashed_Clone(MemCtx *m, Hashed *h);
boolean Hashed_Equals(Hashed *a, Hashed *b);
boolean Hashed_LocalEquals(MemCtx *m, Hashed *a, Hashed *b);
boolean Hashed_ExternalEquals(MemCtx *m, Hashed *a, Hashed *b);
