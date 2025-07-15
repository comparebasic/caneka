typedef struct nested {
    Type type;
    MemCh *m;
    Iter it;
} Nested;

Nested *Nested_Make(MemCh *m);
status Nested_AddByPath(Nested *nd, Span *keys, Abstract *value);
status Nested_AddByKey(Nested *nd, Abstract *key, Abstract *value);
status Nested_SetRoot(Nested *nd, Abstract *root);
Abstract *Nested_Get(Nested *nd);
Abstract *Nested_GetRoot(Nested *nd);
Abstract *Nested_GetByKey(Nested *nd, Abstract *key);
Abstract *Nested_GetByPath(Nested *nd, Span *keys);
Span *Nested_GetPath(Nested *nd);
status Nested_Indent(Nested *nd, Abstract *key);
status Nested_IndentByIdx(Nested *nd, i32 idx);
status Nested_Outdent(Nested *nd);
status Nested_Next(Nested *nd);
