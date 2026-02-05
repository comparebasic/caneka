typedef struct seel_iter {
    Type type;
    Type objType;
    Inst *inst;
    Iter *it;
    IterApi *api;
} InstApi;

InstApi *InstApi_Make(MemCh *m, Inst *inst, Iter *it, IterApi *api);
InstApi *InstApi_From(MemCh *m, Inst *inst);
