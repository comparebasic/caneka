#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Cursor_Tests(MemCh *m){
    status r = READY;
    Str *s;
    StrVec *v;
    Cursor *curs;

    v = StrVec_Make(m);
    curs = Cursor_Make(m, StrVec_Make(m));

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
    void *args[] = {
        I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(delta == 31,
        "Stops on the 32nd y in sky of the same, second Cursor's StrVec, have $", args);

    Cursor_Incr(curs, 3);
    r |= Test(*curs->ptr == 'F', "Stops on the 'F' for For", NULL);
    s = (Str *)Span_Get(curs->v->p, 2);
    delta = curs->ptr - s->bytes;
    void *args2[] = {
        I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(delta == 1,
        "Stops on the 2nd character of the third Cursor's StrVec, have $", args2);

    Cursor_Decr(curs, 5);
    r |= Test(*curs->ptr == 's', "Stops on the 's' for sky", NULL);
    s = (Str *)Span_Get(curs->v->p, 1);
    delta = curs->ptr - s->bytes;
    void *args3[] = {
        I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(delta == 29,
        "Stops on the 29th character of the third Cursor's StrVec, have $", args3);

    return r;
}

status CursorPos_Tests(MemCh *m){
    status r = READY;
    void *args[5];

    Str *s;
    StrVec *v;
    Cursor *curs;

    v = StrVec_Make(m);
    curs = Cursor_Make(m, StrVec_Make(m));

    Cursor_Add(curs, Str_CstrRef(m , "Rain "));
    Cursor_Add(curs, Str_CstrRef(m , "and sunshine, and clouds and sky."));
    Cursor_Add(curs, Str_CstrRef(m , " For a good time, "));
    Cursor_Add(curs, Str_CstrRef(m , "outdoors."));

    args[0] = I64_Wrapped(m, curs->pos);
    args[1] = curs;
    args[2] = NULL;

    r |= Test(curs->pos == -1,
        "Cursor starts at pos:$, @", args);
    
    for(i64 i = 0; i < curs->v->total; i++){
        Cursor_NextByte(curs);
        if(curs->pos != i){
            args[0] = I64_Wrapped(m, i);
            args[1] = I64_Wrapped(m, curs->pos);
            args[2] = NULL;
            r |= Test(curs->pos != i,
                "Cursor Position matches as NextByte iterates over it, expected $, have $", args);
        }
    }

    args[0] = I64_Wrapped(m, curs->pos);
    args[1] = I64_Wrapped(m, (curs->v->total-1));
    args[2] = NULL;
    r |= Test(curs->pos == (curs->v->total-1),
        "Cursor Position matched as NextByte iterated over it,"
        " expected $, have $", args);

    Cursor_Reset(curs);

    i64 i = 0;
    for(; i < 5; i++){
        Cursor_NextByte(curs);
        if(curs->pos != i){
            args[0] = I64_Wrapped(m, i);
            args[1] = I64_Wrapped(m, curs->pos);
            args[2] = NULL;
            r |= Test(curs->pos == i,
                "Cursor Position matches as jumpy NextByte iterates over it,"
                " expected $, have $", args);
        }
    }
    i -= 1;

    i -= 2;
    Cursor_Decr(curs, 2);
    args[0] = I64_Wrapped(m, i);
    args[1] = I64_Wrapped(m, curs->pos);
    args[2] = curs;
    args[3] = NULL;
    r |= Test(curs->pos == i,
        "Cursor Position matched after Decr(2),"
        " expected $, have $, @", args);

    for(i = i+1; i < 10; i++){
        Cursor_NextByte(curs);
        if(curs->pos != i){
            args[0] = I64_Wrapped(m, i);
            args[1] = I64_Wrapped(m, curs->pos);
            args[2] = NULL;
            r |= Test(curs->pos == i,
                "Cursor Position matches as jumpy NextByte iterates over it,"
                " expected $, have $", args);
        }
    }
    i -= 1;

    i += 4;
    Cursor_Incr(curs, 4);

    args[0] = I64_Wrapped(m, i);
    args[1] = I64_Wrapped(m, curs->pos);
    args[2] = curs;
    args[3] = NULL;
    r |= Test(curs->pos == i,
        "Cursor Position matched after NextByte to (10) and Incr(4),"
        " expected $, have $, @", args);

    Cursor_End(curs);
    args[0] = I64_Wrapped(m, curs->v->total-1);
    args[1] = I64_Wrapped(m, curs->pos);
    args[2] = curs;
    args[3] = NULL;
    r |= Test(curs->pos == (curs->v->total-1),
        "Cursor Position matched at End,"
        " expected $, have $, @", args);

    return r;
}
