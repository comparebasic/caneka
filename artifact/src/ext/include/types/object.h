typedef struct objtype {
    Type type;
    Type objType;
} ObjType;

typedef struct objnative {
    Type type;
    Type objType;
    Abstract *a;
} ObjNative;

typedef struct object {
    Type type;
    Type objType;
    i32 propMask;
    Table *tbl;
    Span *order;
} Object;

#define Object_GetMem(o) ((o)->order->m)

Object *Object_Make(MemCh *m, cls typeOf);
boolean Object_IsBlank(Object *obj);

boolean Object_TypeMatch(Abstract *a, cls typeOf);
Object *Object_As(Object *obj, cls typeOf);

Hashed *Object_Set(Object *obj, Abstract *key, Abstract *value);
Abstract *Object_Get(Object *obj, Abstract *key);

Hashed *Object_SetProp(Object *obj, Str *key);
Abstract *Object_GetProp(Object *obj, Str *key);

Hashed *Object_SetByIdx(Object *obj, Abstract *key, Abstract *value);
Hashed *Object_GetByIdx(Object *obj, i32 idx);
Hashed *Object_SetPropByIdx(Object *obj, i32 idx, Abstract *value);
Hashed *Object_GetPropByIdx(Object *obj, i32 idx);

Object *Object_GetOrMake(Object *obj, Abstract *key);
status Object_AddByPath(Object *obj, StrVec *path, Abstract *value);

Abstract *Object_GetIter(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source);
