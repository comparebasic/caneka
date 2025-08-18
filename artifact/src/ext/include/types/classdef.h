typedef struct classdef {
    Type type;
    Type objType;
    Type parentType;
    Maker make;
    Str *name;
    Str **states; 
    Table *atts;
    Table *methods;
    FetchFunc byKey;
    FetchFunc byIdx;
    FetchFunc getIter;
    ToSFunc toS;
    /*
    boolean Equals
    boolean Exact
    boolean Truthy 
    */
} ClassDef;

extern Lookup *ClassLookup;

status Class_Register(MemCh *m, ClassDef *cls);
ClassDef *ClassDef_Make(MemCh *m);
status ClassDef_Init(MemCh *m);
Abstract *ClassDef_Undefined(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source);
