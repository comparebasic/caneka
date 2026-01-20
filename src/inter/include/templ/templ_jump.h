/* uses fetcher op flags */

typedef struct templ_jump {
    Type type;
    Type sourceType;
    Type retType;
    Fetcher *fch;
    i32 idx;
    struct {
        TemplCrit dest;
        TemplCrit skip;
        TemplCrit enclose;
        TemplCrit out;
    } crit;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch);
status Templ_HandleJump(Templ *templ);
