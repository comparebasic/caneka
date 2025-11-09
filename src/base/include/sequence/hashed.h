enum hash_flags {
    FLAG_HASHED_RAW = 1 << 11,
};

typedef struct hashed {
    Type type;
    i32 idx;
    i32 orderIdx;
    util id;
    void *key;
    void *value;
} Hashed;

extern struct lookup *HashLookup;
typedef util (*HashFunc)(void *a);

util Hash_Bytes(byte *bt, size_t length);
util Get_Hash(void *a);
boolean Hashed_Equals(Hashed *a, Hashed *b);
Hashed *Hashed_Make(MemCh *m, void *a);
Hashed *Hashed_Clone(MemCh *m, Hashed *oh);
status Hash_Init(MemCh *m);
