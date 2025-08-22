typedef struct object {
    Type type;
    Type objType;
    i32 propMask;
    Table *tbl;
    Span *order;
} Object;

Object *Object_Make(MemCh *m, cls typeOf);
boolean Object_IsBlank(Object *obj);

Hashed *Object_Set(Object *obj, Abstract *key, Abstract *value);
Abstract *Object_Get(Object *obj, Abstract *key);

Hashed *Object_SetProp(Object *obj, Str *key);
Abstract *Object_GetProp(Object *obj, Str *key);

Hashed *Object_SetByIdx(Object *obj, Abstract *key, Abstract *value);
Hashed *Object_GetByIdx(Object *obj, i32 idx);
Hashed *Object_SetPropByIdx(Object *obj, Str *key);
Hashed *Object_GetPropByIdx(Object *obj, Str *key);

Object *Object_GetOrMake(Object *obj, Abstract *key);
status Object_AddByPath(Object *obj, StrVec *path, Abstract *value);

Iter *Object_GetIter(Object *o);
