/* uses fetcher op flags */

typedef struct templ_jump {
    Type type;
    Type sourceType;
    Fetcher *fch;
    i32 idx;
    struct {
        TemplCrit dest;
        TemplCrit skip;
        TemplCrit ret;
        TemplCrit enclose;
    } crit;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch);
status Templ_HandleJump(Templ *templ);
