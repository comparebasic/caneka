typedef struct seel_iter {
    Type type;
    Type objType;
    Inst *inst;
    Iter *it;
    IterApi *api;
} InstIter;

InstIter *InstIter_Make(MemCh *m, Inst *inst, Iter *it, IterApi *api);
InstIter *InstIter_From(MemCh *m, Inst *inst);
