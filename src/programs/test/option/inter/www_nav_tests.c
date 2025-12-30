#include <external.h>
#include <caneka.h>

status WwwNav_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    NodeObj *node = Inst_Make(m, TYPE_NODEOBJ);

    StrVec *path = IoPath_From(m, S(m, "/docs/base/bytes/str"));
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    Seel_Set(item, K(m, "name"), S(m, "Str"));
    Inst_ByPath(node, path, item, SPAN_OP_SET);

    path = IoPath_From(m, S(m, "/docs/base/bytes/fmt"));
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    Seel_Set(item, K(m, "name"), S(m, "Fmt"));
    Inst_ByPath(node, path, item, SPAN_OP_SET);

    path = IoPath_From(m, S(m, "/docs/base/bytes/tos"));
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    Seel_Set(item, K(m, "name"), S(m, "ToS"));
    Inst_ByPath(node, path, item, SPAN_OP_SET);

    path = IoPath_From(m, S(m, "/docs/base/mem/span"));
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    Seel_Set(item, K(m, "name"), S(m, "Span"));
    Inst_ByPath(node, path, item, SPAN_OP_SET);

    path = IoPath_From(m, S(m, "/docs/base/mem/memch"));
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    Seel_Set(item, K(m, "name"), S(m, "MemCh"));
    Inst_ByPath(node, path, item, SPAN_OP_SET);

    args[0] = node;
    args[1] = NULL;
    Out("^p.Node @^0\n", args);

    DebugStack_Pop();
    return r;
}
