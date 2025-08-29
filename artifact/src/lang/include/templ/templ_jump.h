/* usese fetcher op flags */

typedef struct templ_jump {
    Type type;
    Type jumpType;
    i32 idx;
    i32 destIdx;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, i32 idx, i32 destIdx);
