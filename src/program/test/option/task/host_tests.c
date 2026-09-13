#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Host_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Str *name = S(m, "firecrow.com");
    HostEnt *ent = HostEnt_FromName(m, name, S(m, "http"));
    r |= Test((ent->type.state & ERROR) == 0, 
        "Hostname lookup succeeded with out an error flag", args);

    if(r & ERROR){
        Return(m, r);
    }

    Str *expected = S(m, "71.19.150.127"); 
    Single *addr = (Single *)ent->addr;

    args[0] = ent->name;
    args[1] = expected;
    args[2] = Ip4_ToStr(m, addr->val.i);
    args[3] = NULL;
    r |= Test(addr->val.i == Str_ToIp4(m, expected), 
        "Ip Address for $ matches, expected $, have $", args);

    Return(m, r);
}
