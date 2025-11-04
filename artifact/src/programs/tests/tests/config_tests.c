#include <external.h>
#include <caneka.h>

status Config_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/object.config"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    Roebling *rbl = FormatConfig_Make(m, curs, NULL);
    Roebling_Run(rbl);

    Object *root = FormatConfig_GetRoot(rbl);;
    args[0] = (Abstract *)root;
    args[1] = NULL;
    Out("^p.root: @^0\n", args);

    r |= ERROR;
    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
