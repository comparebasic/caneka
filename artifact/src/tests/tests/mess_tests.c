#include <external.h>
#include <caneka.h>

char *fmtCstr = ""
    "= Hidy Ho\n"
    "\n"
    "And this is a first paragraph\n"
    "with a second line.\n"
    "\n"
    "Second single sentance with details _link=here@http://example.com.\n"
    "\n"
    "_image=Image one@image.png"
    "\n"
    "- bullet one.\n"
    "- bullet two\nwith two lines.\n"
    "- bullet three\n  with two lines as well.\n"
    "- bullet four.\n"
    "\n"
    "== Sub\n"
    "Section\n"
    "\n"
    "A paragraph in the sub section.\n"
    "\n"
    "= Table Title\n"
    "\n"
    "+Column A,Column B,Column C\n"
    "Apple, 1, 37\n"
    "Bannana, 2, 39\n"
    "Cantelope, 3, 39\n"
    "\n"
    "And the final text here.\n"
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
    Roebling_Run(rbl);

    Abstract *args[] = {
        (Abstract *)rbl->mess,
        NULL
    };
    Debug("^y.Mess @^0.\n", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
