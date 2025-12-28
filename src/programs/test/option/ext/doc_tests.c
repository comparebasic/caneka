#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Doc_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    StrVec *src = IoAbsPath(m, "src");
    DocComp *comp = DocComp_FromStr(m, src, S(m, "base.bytes.Str"));

    Str *path = StrVec_Str(m, Inst_Att(comp, K(m, "src")));
    StrVec *content = File_ToVec(m, path);


    Cursor *curs = Cursor_Make(m, content);
    Roebling *rbl = Doc_MakeRoebling(m, curs, comp);
    Roebling_Run(rbl);

    args[0] = comp;
    args[1] = path;
    args[2] = NULL;
    Out("^p.doc object @\n@^0\n", args);

    DebugStack_Pop();
    return r;
}
