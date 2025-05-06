#include <external.h>
#include <caneka.h>

char *fmtCstr = ""
    "= Hidy Ho\n"
    "\n"
    "And this is a first paragraph\n"
    "with a second line."
    "\n"
    "Second single sentatnce."
    "\n"
;

status Mess_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, fmtCstr));
    Cursor *curs = Cursor_Make(m, v);

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    rbl->type.state |= DEBUG;
    Roebling_Run(rbl);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
