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
    
    status re = Fmt_ToHtml(sm, rbl->mess);
    r |= Test(re & SUCCESS, "Fmt to HTML has status SUCCESS\n", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
