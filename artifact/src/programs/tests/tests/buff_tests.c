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
    r |= Test(Equals((Abstract *)bf->v, (Abstract *)expected),
        "Content is expected in StrVec of Buff, expected @, have @", args);

    char *longStr = "\nThe weather is under my control now... MY CONTROL. "
    "and just to make it super clear what that means I'm going to repeat myself"
    " unecissarily to make this paragraph REALLY LONG! MY CONTROL,  MY CONTROL, "
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

    Str *s = Str_CstrRef(m, "All of the things you want to know are kindof like "
        "dandilions.");

    Buff *bf = Buff_Make(m, BUFF_STRVEC);
    Send_Add(bf, s);

    Str *expected = s;
    args[0] = (Abstract *)s;
    args[1] = (Abstract *)bf->v;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)bf->v, (Abstract *)expected),
        "Content is expected in StrVec of Buff, expected @, have @", args);

    StrVec *v = StrVec_From(m, Str_CstrRef(m, " They make sense until the wind blows"));
    StrVec_Add(v, Str_CstrRef(m, ", and then they fly away."));
    StrVec_Add(v, Str_CstrRef(m, " Scattered to the far reaches of the hills."));
    StrVec_Add(v, Str_CstrRef(m, "and difficult to make sense of."));

    Send_AddVec(bf, v);

    StrVec *expectedVec = StrVec_From(m, s);
    StrVec_AddVec(expectedVec, v);

    args[0] = (Abstract *)expectedVec;
    args[1] = (Abstract *)bf->v;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)bf->v, (Abstract *)expectedVec),
        "Content with a long additional is expected in StrVec of Buff, expected &, have &", args);

    StrVec *content = StrVec_Make(m);
    Str *shelf = Str_Make(m, 128);
    i16 g = 0;
    while((Recv_Get(bf, shelf) & END) == 0){
        Guard_Incr(bf->m, &g, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
        StrVec_Add(content, shelf);
        if((bf->type.state & LAST) == 0){
            shelf = Str_Make(m, 128);
        }
    }

    bf->v->type.state |= DEBUG;
    args[0] = (Abstract *)bf->v;
    args[1] = (Abstract *)content;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)bf->v, (Abstract *)expectedVec),
        "Recv_Get has populated an equal StrVec, buff &, content &", args);

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
