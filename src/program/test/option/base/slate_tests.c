#include <external.h>
#include <caneka.h>

status Slate_Tests(MemCh *m){
    void *args[3];
    status r = READY;
    m->level++;

    Slate *slate = Slate_Make(m, 4);

    args[0] = slate;
    args[1] =  NULL;
    Out("^p.Slate @ after Make^0\n", args);

    Slate_Add(m, slate, S(m, "poo"));

    args[0] = slate;
    args[1] = NULL;
    Out("^p.Slate @ after adding 'poo'^0\n", args);

    i32 twoIdx = Slate_Add(m, slate, S(m, "two"));
    Slate_Add(m, slate, S(m, "three"));
    Slate_Remove(m, slate, twoIdx);

    args[0] = slate;
    args[1] = NULL;
    Out("^p.Slate @ after adding two, three, removing two^0\n", args);

    Slate_Add(m, slate, S(m, "two2"));

    args[0] = slate;
    args[1] = NULL;
    Out("^p.Slate @ after adding two... again^0\n", args);

    i16 fourIdx = Slate_Add(m, slate, S(m, "four"));

    args[0] = slate;
    args[1] = NULL;
    Out("^p.Slate @ after adding four^0\n", args);

    Slate_Remove(m, slate, 0);

    args[0] = slate;
    args[1] = NULL;
    Out("^p.Slate @ after removing poo^0\n", args);


    r |= ERROR;

    return r;
}
