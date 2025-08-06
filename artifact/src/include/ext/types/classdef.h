
typedef struct classdef {
    Type type;
    Maker make;
    FetchFunc byKey;
    FetchFunc byIdx;
    FetchFunc getIter;
    ToSFunc toS;
    Table *attsTbl;
    Table *methodsTbl;
    Str **states; 
} ClassDef;

extern Lookup *ClassLookup;

ClassDef *ClassDef_Make(MemCh *m);
status ClassDef_Init(MemCh *m);
