typedef struct jump_crit {
    Type type;
    i32 incr;
    i32 idx;
} TemplCrit;

TemplCrit *TemplCrit_Make(MemCh *m, i32 idx, status flags);
