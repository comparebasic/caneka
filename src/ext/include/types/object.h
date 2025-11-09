typedef struct objtype {
    Type type;
    Type objType;
} ObjType;

typedef struct objnative {
    Type type;
    Type objType;
    void *a;
} ObjNative;

typedef struct object {
    Type type;
    Type objType;
    word _;
    i16 depth;
    i32 propMask;
    Table *tbl;
    Span *order;
} Object;

#define Object_GetMem(o) ((o)->order->m)

Object *Object_Make(MemCh *m, cls typeOf);
boolean Object_IsBlank(Object *obj);

boolean Object_TypeMatch(void *a, cls typeOf);
Object *Object_As(Object *obj, cls typeOf);

Hashed *Object_Set(Object *obj, void *key, void *value);
Hashed *Object_GetHashed(Object *obj, void *key);
void *Object_Get(Object *obj, void *key);

Hashed *Object_SetProp(Object *obj, Str *key, void *value);
void *Object_GetProp(Object *obj, Str *key);

Hashed *Object_SetByIdx(Object *obj, void *key, void *value);
Hashed *Object_GetByIdx(Object *obj, i32 idx);
Hashed *Object_SetPropByIdx(Object *obj, i32 idx, void *value);
void *Object_GetPropByIdx(Object *obj, i32 idx);

Object *Object_GetOrMake(Object *obj, void *key, word op);
Object *Object_ByPath(Object *obj, StrVec *path, void *value, word op);

void *Object_GetIter(MemCh *m, FetchTarget *fg, void *data, void *source);
status Object_Depth(void *a);
i32 Object_Add(Object *obj, void *value);

Object *Object_Filter(Object *obj, SourceFunc func, void *source);
