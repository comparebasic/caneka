extern struct lookup * InstIterFuncLookup;

typedef struct seel_iter {
    Type type;
    Type objType;
    Inst *inst;
    Iter *it;
    NextFunc func;
} InstIter;

InstIter *InstIter_Make(MemCh *m, Inst *inst, NextFunc func);
status InstIter_Init(MemCh *m);
InstIter *InstIter_From(MemCh *m, Inst *inst);
