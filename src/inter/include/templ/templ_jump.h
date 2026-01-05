/* usese fetcher op flags */

typedef struct templ_jump {
    Type type;
    Fetcher *fch;
    i16 incr;
    i32 idx;
    i32 destIdx;
    i32 skipIdx;
    i32 tempIdx;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch);
status Templ_HandleJump(Templ *templ);
