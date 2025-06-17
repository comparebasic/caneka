#include <external.h>
#include <caneka.h>

status FmtHtml_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 

    Str *path = File_GetAbsPath(m, Str_CstrRef(m, "./examples/example.fmt"));
    File *f = File_Make(m, path, NULL);
    File_Read(f, FILE_READ_MAX);

    Cursor *curs = File_GetCurs(f);

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    Roebling_Run(rbl);

    Stream *sm = Stream_MakeStrVec(m); 

    Abstract *args[] = {
        (Abstract *)rbl->mess,
        NULL
    };
    Debug("^y.Mess: @^0.\n", args);
    
    Fmt_ToHtml(sm, rbl->mess);
    r |= Test(rbl->mess->transp->type.state & SUCCESS, 
        "Fmt to HTML has status SUCCESS\n", NULL);

    Abstract *args1[] = {
        (Abstract *)rbl->mess->transp,
        NULL
    };
    Debug("^y.@^0.\n", args1);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
