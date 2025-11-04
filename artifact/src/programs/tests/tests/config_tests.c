#include <external.h>
#include <caneka.h>

status Config_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/object.config"));
    StrVec *content = File_ToVec(m, path);

    args[0] = (Abstract *)content;
    args[1] = NULL;
    Out("^p.content: @^0\n", args);

    Cursor *curs = Cursor_Make(m, content);
    Roebling *rbl = FormatConfig_Make(m, curs, NULL);
    rbl->dest->type.state |= DEBUG;
    Roebling_Run(rbl);


    Object *root = Span_Get(((Iter *)rbl->dest)->p, 0);
    args[0] = (Abstract *)root;
    args[1] = NULL;
    Out("^p.root: @^0\n", args);

    r |= ERROR;
    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
