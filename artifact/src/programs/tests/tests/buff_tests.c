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
    Buff_Add(bf, s);

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

    Buff_AddVec(bf, v);

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
    while((Buff_GetStr(bf, shelf) & END) == 0){
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
        "Buff_GetStr has populated an equal StrVec, buff &, content &", args);

    MemCh_Free(m);
    DebugStack_Pop();

    return r;
}

status BuffIo_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();

    StrVec *path = IoUtil_AbsVec(m, StrVec_From(m, Str_CstrRef(m, "examples/test/")));
    Dir_CheckCreate(m, StrVec_Str(m, path));

    StrVec *pathOne = StrVec_Copy(m, path);

    StrVec *fname = StrVec_From(m, Str_CstrRef(m, "buffio.txt"));
    IoUtil_Add(m, pathOne, fname);

    i32 fd = open(Str_Cstr(m, StrVec_Str(m, pathOne)), (O_CREAT|O_WRONLY|O_TRUNC), 0644);
    args[0] = (Abstract *)I32_Wrapped(m, fd);
    args[1] = (Abstract *)pathOne;
    args[2] = NULL;
    r |= Test(fd > 0,
        "File opened successfully to create and write have ^D.$^d.fd for @", args);

    Buff *out = Buff_Make(m, BUFF_UNBUFFERED);
    Buff_SetFd(out, fd);

    char *content = "Hidy Ho, my friend, this is a nice place to be. where"
    " things are peachy and wonderous. Yay woundersous is as good a word as"
    " walrus. If you're reading this, then something is probably " "broken, and"
    " your looking at the test. I'm so sorry it's broken. But we believe you can"
    " fix it. Yay.";
    i64 offset = 0;
    Buff_Unbuff(out, (byte *)content, strlen(content), &offset);
    close(fd);
    Buff_UnsetFd(out);

    args[0] = (Abstract *)out;
    args[1] = NULL;
    r |= Test(out->v->total == 0, "Buff total is still 0 after sending content "
        "through it unbuffered: @", args);

    r |= Test(offset == strlen(content), "total offset matches content length ", args);

    i32 ifd = open(Str_Cstr(m, StrVec_Str(m, pathOne)), O_RDONLY);
    Buff *chain = Buff_Make(m, ZERO);
    Buff_SetFd(chain, ifd);
    args[0] = (Abstract *)I32_Wrapped(m, fd);
    args[1] = (Abstract *)pathOne;
    args[2] = NULL;
    r |= Test(fd > 0,
        "File opened successfully to after write, have ^D.$^d.fd for @", args);

    StrVec *pathTwo = StrVec_Copy(m, path);
    fname = StrVec_From(m, Str_CstrRef(m, "buffio-compare.txt"));
    IoUtil_Add(m, pathTwo, fname);

    i32 ofd = open(Str_Cstr(m, StrVec_Str(m, pathTwo)), (O_CREAT|O_WRONLY|O_TRUNC), 0644);
    args[0] = (Abstract *)I32_Wrapped(m, ofd);
    args[1] = (Abstract *)pathTwo;
    args[2] = NULL;
    r |= Test(ofd > 0,
        "Second file opened successfully to create and write have ^D.$^d.fd for @", args);

    Buff_Read(chain);
    Buff_SendToFd(chain, ofd);
    close(ifd);
    Buff_UnsetFd(chain);
    close(ofd);

    Buff *compare = Buff_Make(m, ZERO);
    i32 rfd = open(Str_Cstr(m, StrVec_Str(m, pathTwo)), O_RDONLY);
    args[0] = (Abstract *)I32_Wrapped(m, rfd);
    args[1] = (Abstract *)pathTwo;
    args[2] = NULL;
    r |= Test(rfd > 0,
        "Second file opened successfully to create for reading have ^D.$^d.fd for @", args);
    Buff_SetFd(compare, rfd);
    Buff_Read(compare);
    close(rfd);

    args[0] = (Abstract *)I32_Wrapped(m, fd);
    args[1] = (Abstract *)pathTwo;
    args[2] = NULL;
    r |= Test(fd > 0,
        "File opened successfully to after write, have ^D.$^d.fd for @", args);

    Str *expected = Str_CstrRef(m, content);
    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)compare->v;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)compare->v, (Abstract *)expected),
        "After writing, reading, writing somewhere else, and then reading it again, "
        "it all equals, expected @, have @", args);

    args[0] = (Abstract *)pathOne;
    args[1] = NULL;
    r |= Test(IoUtil_Unlink(StrVec_Str(m, pathOne)) & SUCCESS, 
        "Remove file @", args);

    args[0] = (Abstract *)pathTwo;
    args[1] = NULL;
    r |= Test(IoUtil_Unlink(StrVec_Str(m, pathTwo)) & SUCCESS, 
        "Remove file @", args);

    MemCh_Free(m);
    DebugStack_Pop();

    return r;
}

status BuffPos_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();
    Buff *bf = Buff_Make(m, BUFF_STRVEC);

    Buff_Add(bf, Str_CstrRef(m, "Once upon a time..."));
    Buff_Add(bf, Str_CstrRef(m, "There was a prince, a queen, a frog, and"));
    Buff_Add(bf, Str_CstrRef(m, " a book."));

    args[0] = (Abstract *)bf;
    args[1] = NULL;
    r |= Test(bf->unsent.total == 67, "Buff: Expected position and unset $", args);

    Buff_Pos(bf, 10);
    r |= Test(bf->unsent.total == 57, "Buff +10: Expected position and unset $", args);

    Buff_PosAbs(bf, -10);
    r |= Test(bf->unsent.total == 10, "Buff -10(abs): Expected position and unset $", args);

    Buff_Pos(bf, -20);
    r |= Test(bf->unsent.total == 30, "Buff -20: Expected position and unset $", args);

    Buff_Pos(bf, 3);
    r |= Test(bf->unsent.total == 27, "Buff +3: Expected position and unset $", args);

    Buff_PosAbs(bf, 7);
    r |= Test(bf->unsent.total == 60, "Buff 7(abs): Expected position and unset $", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
