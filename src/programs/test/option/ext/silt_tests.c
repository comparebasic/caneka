#include <external.h>
#include <caneka.h>
#include <test_module.h>

static enum domains {
    DOMAIN_ONE = 1,
    DOMAIN_TWO = 2,
};

static enum protos {
    PROTO_PLAIN = 1,
    PROTO_TLS = 2,
};

status Silt_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Task *tsk = Inst_Make(m, TYPE_TASK); 
    Seel_Set(nd, K(m, "core"), Make_Req(m));
    Seel_Set(nd, K(m, "io"), ReqHttp_Make(m));
    Seel_Set(nd, K(m, "proto"), I16_Wrapped(m, PROTO_TLS));
    Seel_Set(nd, K(m, "domain"), I16_Wrapped(m, DOMAIN_TWO));

    Str *silt = Inst_GetSilt(m, nd);
    Buff *bf = Buff_Make(m, ZERO);
    Silt_Print(bf, silt, TYPE_INST_SILT, ZERO);

    Str *expected = S(m, "Silt<TYPE_TASK, TYPE_TASK_CORE, TYPE_HTTP_REQ, 2, 1>");

    args[0] = expected;
    args[1] = bf->v;
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Silt Str has expected value expected @, have @", args);

    Return(m, r);
}
