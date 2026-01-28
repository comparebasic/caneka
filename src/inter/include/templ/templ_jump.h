/* uses fetcher op flags */

typedef struct jumps {
    Type type;
    i32 idx;
    TemplCrit *crit[8];
    TemplFunc *funcs[8];
} Jumps;

Jumps *Jumps_Make(MemCh *m, i32 idx);
status Templ_HandleJump(Templ *templ);
