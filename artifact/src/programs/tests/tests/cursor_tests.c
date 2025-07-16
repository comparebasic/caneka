#include <external.h>
#include <caneka.h>
#include <tests.h>

status Cursor_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    StrVec *v;
    Cursor *curs;

    v = StrVec_Make(m);
    curs = Cursor_Make(m, StrVec_Make(m));
    curs->type.state |= DEBUG;

    Cursor_Add(curs, Str_CstrRef(m , "Rain "));
    Cursor_Add(curs, Str_CstrRef(m , "and sunshine, and clouds and sky."));
    Cursor_Add(curs, Str_CstrRef(m , " For a good time, "));
    Cursor_Add(curs, Str_CstrRef(m , "outdoors."));

    for(i32 i = 0; i < 7; i ++){
        Cursor_NextByte(curs);
    }

    r |= Test(curs->ptr != NULL, "Ptr is not NULL", NULL);
    r |= Test(*curs->ptr == 'n', "Stops on the first 'n' for and", NULL);
    s = (Str *)Span_Get(curs->v->p, 1);
    r |= Test(curs->ptr - s->bytes == 1,
        "Stops on the second character of the second string in the Cursor's StrVec", NULL);

    Cursor_Incr(curs, 30);

    r |= Test(*curs->ptr == 'y', "Stops on the 'y' for sky", NULL);
    i64 delta = curs->ptr - s->bytes;
    Abstract *args[] = {
        (Abstract *)I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(delta == 31,
        "Stops on the 32nd y in sky of the same, second Cursor's StrVec, have $", args);

    Cursor_Incr(curs, 3);
    r |= Test(*curs->ptr == 'F', "Stops on the 'F' for For", NULL);
    s = (Str *)Span_Get(curs->v->p, 2);
    delta = curs->ptr - s->bytes;
    Abstract *args2[] = {
        (Abstract *)I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(delta == 1,
        "Stops on the 2nd character of the third Cursor's StrVec, have $", args2);

    Cursor_Decr(curs, 5);
    r |= Test(*curs->ptr == 's', "Stops on the 's' for sky", NULL);
    s = (Str *)Span_Get(curs->v->p, 1);
    delta = curs->ptr - s->bytes;
    Abstract *args3[] = {
        (Abstract *)I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(delta == 29,
        "Stops on the 29th character of the third Cursor's StrVec, have $", args3);

    MemCh_Free(m);
    return r;
}
