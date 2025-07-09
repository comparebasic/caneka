typedef struct templ_jump {
    Type type;
    Type jumpType;
    i32 idx;
    i32 destIdx;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, cls typeOf, i32 idx, i32 destIdx);
