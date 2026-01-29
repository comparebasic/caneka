typedef struct jump_crit {
    Type type;
    DoubleFlag dflag;
    i32 contentIdx;
    i32 dataIdx;
} TemplCrit;

TemplCrit *TemplCrit_Make(MemCh *m, i32 idx, status positive, status negative);
