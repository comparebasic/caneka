#include <external.h>
#include <caneka.h>

status Buff_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();

    Buff *bf = Buff_Make(m, BUFF_STRVEC);

    char *cstr = "Hi, hows the weather";
    Buff_AddBytes(bf, (byte *)cstr, strlen(cstr)); 

    cstr = ", is it cold?";
    Buff_AddBytes(bf, (byte *)cstr, strlen(cstr)); 

    cstr = " and StOrMy, Mwahahahahaha!";
    Buff_AddBytes(bf, (byte *)cstr, strlen(cstr)); 

    Str *expected = Str_CstrRef(m, "Hi, hows the weather, is it cold?"
        " and StOrMy, Mwahahahahaha!");

    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)bf->v;
    args[2] = NULL;
    bf->v->type.state |= DEBUG;
    r |= Test(Equals((Abstract *)bf->v, (Abstract *)expected),
        "Content is expected in StrVec of Buff, expected @, have @", args);

    char *longStr = "\nThe weather is under my control now... MY CONTROL. "
    "and just to make it super clear what that means I'm going to repeat myself"
    "unecissarily to make this paragraph REALLY LONG! MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL"
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY "
    "CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL, "
    "MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL,  MY CONTROL... mine.";

    Buff_AddBytes(bf, (byte *)longStr, strlen(longStr)); 

    StrVec *expectedVec = StrVec_From(m, expected);
    StrVec_Add(expectedVec, Str_CstrRef(m, longStr));

    args[0] = (Abstract *)expectedVec;
    args[1] = (Abstract *)bf->v;
    args[2] = NULL;
    bf->v->type.state |= DEBUG;
    r |= Test(Equals((Abstract *)bf->v, (Abstract *)expectedVec),
        "Content with a long additional is expected in StrVec of Buff, expected &, have &", args);

    MemCh_Free(m);
    DebugStack_Pop();

    return r;
}

status BuffSendRecv_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();

    MemCh_Free(m);
    DebugStack_Pop();

    return r;
}

status BuffIo_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();

    MemCh_Free(m);
    DebugStack_Pop();

    r |= ERROR;
    return r;
}
