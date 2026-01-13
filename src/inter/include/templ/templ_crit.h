typedef struct jump_crit {
    Type type;
    i32 incr;
    i32 idx;
} TemplCrit;

TemplCrit *TemplCrit_Make(MemCh *m);
i32 TemplCrit_Decr(Templ *templ, status flags);
status TemplCrit_Add(Templ *templ, i32 idx, status flags);
status TemplCrit_Reset(Templ *templ);
