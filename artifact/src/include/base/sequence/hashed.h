enum hash_flags {
    FLAG_HASHED_RAW = 1 << 11,
    FLAG_HASHED_COLLISION = 1 << 12,
};

typedef struct hashed {
    Type type;
    int idx;
    util id;
    Abstract *item;
    Abstract *value;
} Hashed;

extern struct lookup *HashLookup;
typedef util (*HashFunc)(Abstract *a);

util Hash_Bytes(byte *bt, size_t length);
util Get_Hash(Abstract *a);
boolean Hashed_Equals(Hashed *a, Hashed *b);
Hashed *Hashed_Make(MemCh *m, Abstract *a);
Hashed *Hashed_Clone(MemCh *m, Hashed *oh);
status Hash_Init(MemCh *m);
