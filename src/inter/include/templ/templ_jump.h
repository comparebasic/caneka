/* usese fetcher op flags */

typedef struct jump_crit {
    Type type;
    i32 incr;
    i32 idx;
} JumpCrit;

typedef struct templ_jump {
    Type type;
    Type sourceType;
    Fetcher *fch;
    i32 idx;
    struct {
        JumpCrit dest;
        JumpCrit skip;
        JumpCrit ret;
        JumpCrit enclose;
    } crit;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch);
status Templ_HandleJump(Templ *templ);
