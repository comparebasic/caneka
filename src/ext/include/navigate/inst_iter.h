extern struct lookup * IterApiLookup;

typedef struct inst_iter_api {
    Type type;
    IterFunc next;
    IterFunc prev;
    IterGetFunc get;
    Abstract *source;
} IterApi;

typedef struct seel_iter {
    Type type;
    Type objType;
    Inst *inst;
    Iter *it;
    IterApi *api;
} InstIter;

InstIter *InstIter_Make(MemCh *m, Inst *inst, Iter *it, IterApi *api);
status InstIter_Init(MemCh *m);
InstIter *InstIter_From(MemCh *m, Inst *inst);

IterApi *IterApi_Make(MemCh *m, IterFunc next, IterFunc prev, IterGetFunc get);
