extern struct lookup *TemplFuncLookup;

typedef struct jump_func {
    Type type;
    Func *func;
} TemplFunc;

TemplFunc *TemplFunc_Make(MemCh *m, Func *, status flags);
