#include <external.h>
#include <caneka.h>

status WwwNav_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    NodeObj *node = Inst_Make(m, TYPE_NODEOBJ);
    Table *coordTbl = Table_Make(m);

    StrVec *path = IoPath_From(m, S(m, "/docs/base/bytes/str"));
    Span *coords = Span_Make(m);
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    StrVec *name = Sv(m, "Str");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/fmt"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Fmt");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/tos"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "ToS");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/mem/span"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Span");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/mem/memch"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "MemCh");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    Str *s = S(m, "Fmt");
    Span *crd = Table_Get(coordTbl, s);
    Iter *it = Iter_Make(m, NULL);
    NestSel_Init(it, node, crd);

    args[0] = s;
    args[1] = crd;
    args[2] = NULL;
    Out("^p.Str @\nCrd @^0\n", args);

    while((NestSel_Next(it) & END) == 0){
        Abstract *a = NestSel_Get(it);
        args[0] = a;
        args[1] = I32_Wrapped(m, it->idx);
        args[2] = NULL;
        Out("^p.  Item @ level @^0\n", args);
    }

    DebugStack_Pop();
    return r;
}
