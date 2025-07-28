#include <external.h>
#include <caneka.h>
#include <tests.h>

status Str_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Str *s;
    s = Str_CstrRef(m, "Hi");
    status r = READY;
    cls type = TYPE_STR;
    Abstract *args1[] = {
        (Abstract *)Type_ToStr(m, type),
        (Abstract *)s,
        NULL
    };
    r |= Test(s->type.of == TYPE_STR, "Expect string to have fixed type $ found @", args1);
    i32 two = 2;
    Abstract *args2[] = {
        (Abstract *)I32_Wrapped(m, two),
        (Abstract *)I32_Wrapped(m, s->length),
        NULL
    };
    r |= Test(s->length == 2, "Expect string length of @ found @", args2);
    Abstract *args3[] = {
        (Abstract *)Str_CstrRef(m, "Hi"),
        (Abstract *)s,
        NULL
    };
    r |= Test(strncmp((char *)s->bytes, "Hi\0", 3) == 0, "Expect string match of @ found @", args3);

    i64 value = 35072;
    s = Str_FromI64(m, value);
    Str *expected_is = Str_CstrRef(m, "35072");
    Abstract *args6[] = {
        (Abstract *)I32_Wrapped(m, value), 
        (Abstract *)I16_Wrapped(m, expected_is->length), 
        (Abstract *)I16_Wrapped(m, s->length),
        NULL
    };
    r |= Test(s->length == expected_is->length, "Expect for i32 value $ length of $ found $", args6);
    Abstract *args7[] = {
        (Abstract *)I32_Wrapped(m, value), 
        NULL
    };
    r |= Test(Equals((Abstract *)s, (Abstract *)expected_is) == TRUE, "Expect string match of i32 of $ to string", args7);

    char *cstr = "GET /path.html HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n"
    "Connection: keep-alive\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8\r\n"
    "\r\n";

    s = Str_CstrRef(m, cstr);
    i64 len = strlen(cstr);
    Abstract *args8[] = {
        (Abstract *)I64_Wrapped(m, len),
        (Abstract *)I16_Wrapped(m, s->length),
        NULL
    };
    r |= Test(s->length == strlen(cstr), "Expect length $, have $", args8);

    MemCh_Free(m);
    return r;
}

status Str_EndMatchTests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    Str *s2;
    char *match;

    match = ".c";
    s = Str_CstrRef(m, "file1.c");
    Abstract *args1[] = {
        (Abstract *)s,
        NULL
    };
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.c' matches successfully, had @", args1);

    match = ".cnk";
    s = Str_CstrRef(m, "file1.cnk");
    Abstract *args2[] = {
        (Abstract *)s,
        NULL
    };
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.cnk' matches successfully, had @", args2);

    match = ".c";
    s = Str_Clone(m, s);
    Str_Trunc(s, -2);
    Abstract *args3[] = {
        (Abstract *)s,
        NULL
    };
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.cnk' matches \".c\" after String_Trunc successfully, had @", args3);

    MemCh_Free(m);

    return r;
}
