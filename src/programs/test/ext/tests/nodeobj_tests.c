#include <external.h>
#include <caneka.h>
#include <test.h>
#include "tests.h"

status NodeObj_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[4];

    NodeObj *root = Inst_Make(m, TYPE_NODEOBJ);

    Abstract *children = Seel_Get(root, S(m, "children"));
    r |= Test(children->type.of == TYPE_TABLE, 
        "Children table has been automatically populated", NULL); 

    NodeObj *nobj = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(nobj, S(m, "name"), Sv(m, "one"));
    Inst_ByPath(root, IoPath(m, "/one"), nobj, SPAN_OP_SET);

    nobj = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(nobj, S(m, "name"), Sv(m, "two"));
    Inst_ByPath(root, IoPath(m, "/two"), nobj, SPAN_OP_SET);

    nobj = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(nobj, S(m, "name"), Sv(m, "three"));
    Inst_ByPath(root, IoPath(m, "/three"), nobj, SPAN_OP_SET);

    nobj = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(nobj, S(m, "name"), Sv(m, "four"));
    Inst_ByPath(root, IoPath(m, "/four"), nobj, SPAN_OP_SET);

    StrVec *path = NULL;
    NodeObj *returned = NULL;

    path = IoPath(m, "/one");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "one"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]), 
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    path = IoPath(m, "/two");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "two"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]), 
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    path = IoPath(m, "/three");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "three"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]),
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    path = IoPath(m, "/four");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "four"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]), 
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    nobj = Inst_Make(m, TYPE_NODEOBJ);
    path = IoPath(m, "/two/and/nested/stuff-one");
    Seel_Set(nobj, S(m, "name"), Sv(m, "two-nested-one"));
    Inst_ByPath(root, path, nobj, SPAN_OP_SET);

    nobj = Inst_Make(m, TYPE_NODEOBJ);
    path = IoPath(m, "/two/secondary/stuff-two");
    Seel_Set(nobj, S(m, "name"), Sv(m, "two-nested"));
    Inst_ByPath(root, path, nobj, SPAN_OP_SET);

    nobj = Inst_Make(m, TYPE_NODEOBJ);
    path = IoPath(m, "/six/new-root/stuff-three");
    Seel_Set(nobj, S(m, "name"), Sv(m, "fresh-three"));
    Inst_ByPath(root, path, nobj, SPAN_OP_SET);


    path = IoPath(m, "/two/secondary/stuff-two");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "two-nested"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]), 
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    path = IoPath(m, "/six/new-root/stuff-three");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "fresh-three"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]), 
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    path = IoPath(m, "/two/and/nested/stuff-one");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    args[0] = path;
    args[1] = S(m, "two-nested-one"); 
    args[2] = returned;
    args[3] = NULL;
    r |= Test(Equals(Seel_Get(returned, S(m, "name")), args[1]), 
        "After setting and requesting ByPath NodeObjs match and has expected property @ -> name:@ -> @", args);

    path = IoPath(m, "/two/and/not-found/here");
    returned = Inst_ByPath(root, path, NULL, SPAN_OP_GET);
    r |= Test(returned == NULL, "Missing path should be NULL", args);

    return r;
}
