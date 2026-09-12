#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Histo_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Str *s = S(m, "Yay, Contenty Things!");
    Histo *hst = Histo_Make(m);
    Histo_Feed(hst, s);

    args[0] = s;
    args[1] = hst;
    args[2] = NULL;
    r |= Test(hst->type.state == ZERO,
        "Histo passes with content @ @^0", args);

    s = S(m, "function(){alert('hi')}");
    hst = Histo_Make(m);
    Histo_Feed(hst, s);

    args[0] = s;
    args[1] = hst;
    args[2] = NULL;
    r |= Test(hst->type.state == HISTO_CODE,
        "Histo sets code flag with content @ @^0", args);

    s = S(m, "click <a href=\"http://steel.example.com/?poo=my-wallet\">here!</a>");
    hst = Histo_Make(m);
    Histo_Feed(hst, s);
    args[0] = s;
    args[1] = hst;
    args[2] = NULL;
    r |= Test(hst->type.state == HISTO_CODE,
        "Histo sets code flag with content @ @^0", args);

    Return(m, r);
}
