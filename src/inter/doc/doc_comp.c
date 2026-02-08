#include <external.h>
#include <caneka.h>

void DocComp_FillFolder(DocComp *comp, StrVec *path){
    MemCh *m = comp->m;
    StrVec *txt = StrVec_Copy(m, path);
    IoUtil_AddVec(m, txt, IoPath(m, "doc.txt"));
    StrVec *name = StrVec_SubVec(m, path, 
        Path_FlagLastIdx(path, MORE), 
        Path_FlagLastIdx(path, MORE)-2);

    Seel_Set(comp, K(m, "name"), name);
    Seel_Set(comp, K(m, "src"), txt);
}

NodeObj *DocComp_FromStr(MemCh *m, StrVec *src, StrVec *name){
    Path_DotAnnotate(m, name);
    Str *first = Span_Get(name->p, 0);
    Str_ToTitle(m, first);
    DocComp *dobj = Inst_Make(m, TYPE_DOC_COMPONENT);
    Seel_Set(dobj, K(m, "name"), name);
    StrVec *path = StrVec_Copy(m, src);
    IoUtil_AddDotPath(path, StrVec_Clone(m, name), S(m, "c"));
    Inst_SetAtt(dobj, K(m, "src"), path);
    StrVec *url = WwwUtils_UrlSeg(m, StrVec_Copy(m, name));
    Inst_SetAtt(dobj, K(m, "url"), url);
    return dobj;
}

status DocComp_Init(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "comments"), I16_Wrapped(m, TYPE_SPAN));
    Table_Set(tbl, S(m, "body"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "functions"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "page"), I16_Wrapped(m, TYPE_WWW_PAGE));
    r |= Seel_Seel(m, tbl, S(m, "DocComp"), TYPE_DOC_COMPONENT);
    return r;
}
