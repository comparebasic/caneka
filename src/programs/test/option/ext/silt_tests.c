#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Silt_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Node *nd = Inst_Make(m, TYPE_NODE); 
    Seel_Set(nd, K(m, "name"), S(m, "Samual"));

    Str *silt = Inst_GetSilt(m, nd);
    Buff *bf = Buff_Make(m, ZERO);
    Silt_Print(bf, silt, TYPE_INST_SILT, ZERO);

    Str *expected = S(m, "Silt<TYPE_NODE, TYPE_STR, TYPE_TABLE>");

    args[0] = expected;
    args[1] = bf->v;
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Silt Str has expected value expected @, have @", args);

    Return(m, r);
}
