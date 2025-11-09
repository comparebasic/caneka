enum class_flags {
    CLASS_NATIVE = 1 << 8,
};

typedef struct classdef {
    Type type;
    Type objType;
    Type originType;
    Maker make;
    Str *name;
    Str **states; 
    Table *props;
    Span *propOrder;
    Table *atts;
    struct {
        FetchFunc byKey;
        FetchFunc byIdx;
        FetchFunc getIter;
        ToSFunc toS;
        /*
        boolean Equals
        boolean Exact
        boolean Truthy 
        */
    } api;
} ClassDef;

extern ClassDef *ObjectCls;
extern Lookup *ClassLookup;

status Class_Register(MemCh *m, ClassDef *cls);
ClassDef *ClassDef_Make(MemCh *m);
status ClassDef_Init(MemCh *m);
void *ClassDef_Undefined(MemCh *m, FetchTarget *fg, void *data, void *source);
status Class_SetupProp(ClassDef *cls, Str *key);
status Class_SetProp(struct object *obj, ClassDef *cls, Str *key, void *value);
i32 Class_GetPropIdx(ClassDef *cls, Str *key);
