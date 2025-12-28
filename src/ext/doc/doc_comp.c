#include <external.h>
#include <caneka.h>

NodeObj *DocComp_FromStr(MemCh *m, StrVec *src, Str *s){
    StrVec *v = StrVec_From(m, s);
    Path_DotAnnotate(m, v);
    Str *first = Span_Get(v->p, 0);
    Str_ToTitle(m, first);
    DocComp *dobj = Inst_Make(m, TYPE_DOC_COMPONENT);
    Seel_Set(dobj, K(m, "name"), v);
    StrVec *path = StrVec_Copy(m, src);
    IoUtil_AddDotPath(path, StrVec_Clone(m, v), S(m, "c"));
    Seel_SetAtt(dobj, K(m, "src"), path);
    return dobj;
}

status DocComp_Init(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    r |= Seel_Seel(m, tbl, S(m, "DocComp"), TYPE_DOC_COMPONENT, h->orderIdx);
    return r;
}
