typedef struct jump_crit {
    Type type;
    i16 contentIdx;
    status flags;
    i32 dataIdx;
} TemplCrit;

TemplCrit *TemplCrit_Make(MemCh *m, i32 idx, status flags);
