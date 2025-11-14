#include <external.h>
#include <caneka.h>

status NodeObj_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();

    NodeObj *root = Inst_Make(m, TYPE_NODEOBJ);

    Abstract *children = Seel_Get(root, S(m, "children"));
    r |= Test(children->type.of == TYPE_TABLE, 
        "Children table has been automatically populated", NULL); 

    NodeObj *nobj = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(nobj, S(m, "name"), S(m, "home"));

    NodeObj_ByPath(root, IoPath(m, "/home"), nobj, SPAN_OP_SET);
    NodeObj *returned = NodeObj_ByPath(root, IoPath(m, "/home"), NULL, SPAN_OP_GET);

    r |= Test(returned == nobj, 
        "After setting and requesting ByPath NodeObjs match", NULL);

    r |= ERROR;

    MemCh_Free(m);
    return r;
}
