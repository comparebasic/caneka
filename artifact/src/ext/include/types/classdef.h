enum class_flags {
    CLASS_STRUCT = 1 << 8,
};

typedef struct classdef {
    ClassType type;
    Type originType;
    Maker make;
    Str *name;
    Str **states; 
    Table *props;
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

extern Lookup *ClassLookup;

status Class_Register(MemCh *m, ClassDef *cls);
ClassDef *ClassDef_Make(MemCh *m);
status ClassDef_Init(MemCh *m);
Abstract *ClassDef_Undefined(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source);
status Class_SetupProp(ClassDef *cls, Str *key);
status Class_SetProp(Abstract *inst, ClassDef *cls, Str *key, Abstract *value);
status Class_GetProp(ClassDef *cls, Str *key);
Iter *Class_GetIter(Abstract *inst, ClassDef *cls);
