/* usese fetcher op flags */

typedef struct templ_jump {
    Type type;
    Fetcher *fch;
    Filter *filter;
    i16 depth;
    i16 level;
    i32 idx;
    i32 destIdx;
    i32 skipIdx;
} TemplJump;

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch);
