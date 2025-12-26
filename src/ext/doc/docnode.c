#include <external.h>
#include <caneka.h>

NodeObj *DocNode_FromStr(MemCh *m, StrVec *src, Str *s){
    StrVec *v = StrVec_From(m, s);
    Path_DotAnnotate(m, v);
    NodeObj *dobj = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(dobj, K(m, "name"), v);
    StrVec *path = StrVec_Copy(m, src);
    IoUtil_AddDotPath(path, StrVec_Clone(m, v), S(m, "c"));
    NodeObj_SetAtt(dobj, K(m, "src"), path);
    return dobj;
}
