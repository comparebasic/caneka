#include <external.h>
#include <caneka.h>

status Cash_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 

    Str *path = File_GetAbsPath(m, Str_CstrRef(m, "./docs/html/header.html"));
    File *f = File_Make(m, path, NULL);
    File_Read(f, FILE_READ_MAX);

    Cursor *curs = File_GetCurs(f);

    Roebling *rbl = NULL;
    rbl = Cash_RoeblingMake(m, curs, NULL);
    rbl->mess->type.state |= DEBUG;
    Roebling_Run(rbl);
    Match *mt = (Match *)Iter_Current(&rbl->matchIt);
    if(mt != NULL && mt->type.state & PROCESSING){
        Roebling_Finalize(rbl, mt, SNIP_CONTENT);
    }

    Abstract *args[] = {
        (Abstract *)rbl,
        NULL
    };
    Out("^p.Cash/Rbl: &^0.\n", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
