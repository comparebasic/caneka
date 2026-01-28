extern struct lookup *TemplFuncLookup;
struct jump_func;

typedef void (*TFunc)(Templ *templ, struct jump_func *tfunc);

typedef struct jump_func {
    Type type;
    DoubleFlag dflag;
    TFunc func;
} TemplFunc;

TemplFunc *TemplFunc_Make(MemCh *m, TFunc func, status flags);
