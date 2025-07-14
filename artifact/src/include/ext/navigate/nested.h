enum nested_states {
    NESTED_ORDERED = 1 << 9,
    NESTED_KEY_STORE = 1 << 10,
};

typedef struct nested {
    Type type;
    Iter it;
    Iter itemIt;
} Nested;

Nested *Nested_Make(MemCh *m);
status Nested_AddByPath(Nested *nd, Span *keys, Abstract *value);
status Nested_AddByKey(Nested *nd, Abstract *key, Abstract *value);
status Nested_Add(Nested *nd, Abstract *value);
Abstract *Nested_Get(Nested *nd);
Abstract *Nested_GetRoot(Nested *nd);
Abstract *Nested_GetByKey(Nested *nd, Abstract *key);
Abstract *Nested_Indent(Nested *nd);
status Nested_Next(Nested *nd);
