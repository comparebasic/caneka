typedef struct nested {
    Type type;
    Iter it;
} Nested;

Nested *Nested_Make(MemCh *m);
status Nested_AddByPath(Nested *nd, Span *keys, Abstract *value);
status Nested_AddByKey(Nested *nd, Abstract *key, Abstract *value);
Abstract *Nested_Get(Nested *nd);
Abstract *Nested_GetRoot(Nested *nd);
Abstract *Nested_GetByKey(Nested *nd, Abstract *key);
status Nested_Indent(Nested *nd, Abstract *key);
status Nested_Outdent(Nested *nd);
status Nested_Next(Nested *nd);
